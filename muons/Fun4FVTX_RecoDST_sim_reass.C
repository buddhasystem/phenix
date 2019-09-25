// $Id: Fun4FVTX_RecoDST_sim.C,v 1.7 2014/10/01 16:39:32 slash Exp $
/*!
  mutoo dst analysis loop for mutoo slowsim simulated dst, possibly embeded
  into either simulated or real data dst
*/
void Fun4FVTX_RecoDST_sim_reass(
  int nEvents = 0,
  char *signalfile = "dst_sim.root",
  char *backgroundfile = "DST_0000406540-0000.root", 
  char *pythiafile = "pythia_mn.root",
  int run_number =  405860,
  char *ana_file = "muon_ana_ntuples.root",
  char *dstfile = "dstembed_out.root",
  char *prdffile = "data_out.prdf",
  char *ndstfile = "ndstembed_out.root",
  char *singlepdstout = "singlemuon_embed_pdst.root"
)
{
  // dump configuration
  cout << endl;
  cout << "Fun4Muons_RecoDST_sim" << endl;
  if( signalfile ) cout << "Fun4Muons_RecoDST_sim - signalfile : " << signalfile << endl;
  if( backgroundfile ) cout << "Fun4Muons_RecoDST_sim - backgroundfile : " << backgroundfile << endl;
  if( dstfile ) cout << "Fun4Muons_RecoDST_sim - dstfile : " << dstfile << endl;
  if( prdffile ) cout << "Fun4Muons_RecoDST_sim - prdffile : " << prdffile << endl;
  if( ndstfile ) cout << "Fun4Muons_RecoDST_sim - ndstfile : " << ndstfile << endl;

  bool is_pp = true;
  bool do_quick_reco = true;
  // Select which vertex will be used in the reconstruction
  bool is_sim = true;
  if ( backgroundfile ){
    is_sim = false;
  }
  else {
    is_sim = true;
    do_quick_reco = false;
  }
 
  // use SVX software
  bool use_SVX = true;

  // use FVTX software
  bool use_FVTX = true;
  bool FVTX_mcreco = false;    // Take MC hits, optionally smear, make tracks and fit
  bool FVTX_realreco = true;   // Take FVTX clusters, make coords and tracks and fit

  // dst/ndst/pdst output
  bool write_dst = false;
  bool write_ndst = false;
  bool write_pdst = true;

  // MUTOO evaluator
  bool do_evaluation = true;
  bool fill_mutr_mc_info = true;

  // code profiler
  bool do_profile = false;

  // perfect detector geometry (no dead channels/HV; 100% muid efficiency)
  bool use_perfect_detector = false;

  bool use_RPC = false;
  /*
    when this flag is true, the mutr/muid hit maps merged after the embedding
    are packed again to a "merged" simulated PRDF
  */
  bool write_merged_prdf = false;

  // load libraries
  gSystem->Load("libmutoo_subsysreco");
  gSystem->Load("libfvtx_subsysreco");
  gSystem->Load("libfun4all");
  gSystem->Load("libfun4allfuncs_muons");
  gSystem->Load("libfun4allfuncs" );
  gSystem->Load("libcteval" );
  gSystem->Load("libMWGOO");
  gSystem->Load("libmuon_subsysreco");
  gSystem->Load("libMWG_interface");
  gSystem->Load("libPHHepMCNode.so");

  if( do_profile ) {
    gSystem->Load("libjprof");
    prof *Pr = new prof;
  }

  ///////////////////////////////////////////
  // recoConsts setup
  //////////////////////////////////////////
  recoConsts *rc = recoConsts::instance();
  rc->set_IntFlag("SVXACTIVE", use_SVX);
  rc->set_IntFlag("RUNNUMBER", run_number);

  // Muon ext vtx verbosity
  // this allows to print which vertex is used and its value
  unsigned int use_vertex;
	if (is_sim){
		use_vertex = MuonUnpackSim::SIGNAL; // "SIGNAL", "BACKGROUND" or "RECO"
	}
	else{
		use_vertex = MuonUnpackSim::BACKGROUND; // "SIGNAL", "BACKGROUND" or "RECO"
	}

	TMutExtVtx::get().set_verbosity( MUTOO::NONE );//ALOT );
  if (use_vertex == MuonUnpackSim::SIGNAL){
    TMutExtVtx::get().set_priorities( "SIM", 0 );
  }
  else{
    TMutExtVtx::get().set_priorities( "FVTX", 0 );
    TMutExtVtx::get().set_priorities( "SVX_PRECISE", 1 );
    TMutExtVtx::get().set_priorities( "BBC", 2 );
  }

  // Read the FVTX dead channel map and geometry from the database:
  // changes track finding parameters and vertex weight used in fvtx-mutr match
  TFvtxGlobalParCntrl::set_bool_par("is_pp",is_pp);   
  TFvtxGlobalParCntrl::set_bool_par("use_svx",use_SVX);   
  TFvtxGlobalParCntrl::set_bool_par("quick_muon_reco",do_quick_reco);

  // Determine what DB parameters to read:
  TFvtxGlobalParCntrl::set_bool_par("is_sim",is_sim);
  TFvtxGlobalParCntrl::set_bool_par("deadmap_use_calibration_database",true);
  TFvtxGlobalParCntrl::set_bool_par("deadmap_use_production_map_for_sim",true);
  TFvtxGlobalParCntrl::set_bool_par("geom_use_calibration_database",false);
  if (is_sim){
    TFvtxGlobalParCntrl::set_string_par("geom_root_file_path","./");
    TFvtxGlobalParCntrl::set_string_par("geom_root_file_name","fvtx_geometry.root");
  }
  else{
    TFvtxGlobalParCntrl::set_string_par("geom_root_file_path","./");
    TFvtxGlobalParCntrl::set_string_par("geom_root_file_name","fvtx_geometry.root");
  }

	// database control initialization
	if( use_perfect_detector )
	{
	  // tell muid tube efficiency must be set to 1
	  rc->set_DoubleFlag("MUIOO_TUBE_EFF",1.0);
	  
	  // make perfect mutr detector
	  TMutDatabaseCntrl::set_database_access("disable_HV",false);
	  TMutDatabaseCntrl::set_database_access("attenuated_channels",false);
	  TMutDatabaseCntrl::set_database_access("dead_channels",false);
	} else 
	  {
		/*
		  to have muid inneficiencies turned on you need to have local copies of
		  tube_eff_north_default.txt
		  tube_eff_south_default.txt
		*/
	    
		///////////////////////////////////////////
		// MUID setup
		//////////////////////////////////////////
	    std::string muid_eff_south("muid_tube_eff_south_Run15pp200.txt");
	    std::string muid_eff_north("muid_tube_eff_north_Run15pp200.txt");
	    
	    {
	      std::cout << "using local two pack efficiciency files:" << muid_eff_south << ", "  << muid_eff_north << std::endl;
	      TMuiHVMask::set_mode(TMuiHVMask::FROM_FILE);
	      TMuiHVMask::set_filename_south(muid_eff_south.c_str());
	      TMuiHVMask::set_filename_north(muid_eff_north.c_str());
	    }
	    
	    /*
	      to have mutr HV mask turned on, you need to have the following set to true
	      and a local copy of mut.disabledAnodes.dat
	    */

	    TMutDatabaseCntrl::set_database_access("use_local_dead_HV_file",false); //mut.disabledAnodes.dat
	    TString hv_file = Form("HV/mut.disabledAnodes.dat_run%d",run_number);
	    TMutDatabaseCntrl::set_filename("use_local_dead_HV_file", hv_file.Data());
	    TMutDatabaseCntrl::set_database_access("use_local_dead_wire_file",false); //mut.disabledWires.dat
	    TMutDatabaseCntrl::set_database_access("use_local_attenuated_chnl_file",false); //AttenuatedChannels.txt
	    TMutDatabaseCntrl::set_database_access("use_local_dead_channel_file",false); //DeadChannels.dat
	    TMutDatabaseCntrl::set_database_access("use_local_landau_parameters_file",true);
	    TMutDatabaseCntrl::set_filename("use_local_landau_parameters_file", "landau_parameters.txt");
	    //	    TMutDatabaseCntrl::set_database_access("use_local_st1_autocad_file",true); //0xx_x.dat
	    //	    TMutDatabaseCntrl::set_database_access("use_local_dcm_map_file",true); //mut.St1ChannelMap.North
	    //	    TMutDatabaseCntrl::set_database_access("use_local_octant_survey_file",true); //MutOctantPositions.dat
	  }

  ///////////////////////////////////////////
  // Make the Server
  //////////////////////////////////////////
  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(0);

  ///////////////////////////////////////////
  // Subsystems
  //////////////////////////////////////////

  // counter
  se->registerSubsystem( new MuonCounter() );

  // this part should no be necessary unless you run on an 'old' simulated DST
  // (for which VtxSimreco did not run in Fun4Muons_PISA
  bool use_vtx_sim_reco = false;
  if( use_vtx_sim_reco )
    {
      gSystem->Load("libsimreco_base");
      VtxSimreco* vtx_reco = new VtxSimreco();
      //		vtx_reco->SmearX( false );
      //		vtx_reco->SmearY( false );
      //		vtx_reco->SmearZ( false );
      //		vtx_reco->XVertexSigma( 1.0 ); //500 micron for FVTX-z smearing
      //		vtx_reco->YVertexSigma( 1.0 ); //500 micron for FVTX-z smearing
      //		vtx_reco->ZVertexSigma( 2.0 ); //500 micron for FVTX-z smearing
      vtx_reco->UseXY( true );
      vtx_reco->OverwriteBBC( is_sim );
      se->registerSubsystem( vtx_reco );
    }
  
  // this runs the SVX reconstruction (stand-alone)
  if( use_SVX )
  {
    // VTX Reconstruction software:
    gSystem->Load("libsimreco");
    //gSystem->Load("libembedreco");
    // Private embedding library
    // This is becasue of different TOP_NODE (signal node in VTX sim, and background node in FVTX sim)
    gSystem->Load("/gpfs/mnt/gpfs02/phenix/hhj/hhj1/shlim/work/02.datafiles/01.simfiles/mysetup/embedreco/install/lib/libembedreco");
    gSystem->Load("libsvx");
    
    SvxParManager *svxpar = new SvxParManager();
    svxpar->Verbosity(0);
    svxpar->set_OffsetVtxToCnt(0.0, 0.0, 0.0);
    svxpar->set_OffsetEastToWest(0.0, 0.0, 0.0);
    svxpar->set_ReadGeoParFromFile(1);
    svxpar->set_GeometryFileName("svxPISA.par");
    se->registerSubsystem(svxpar);
    
    if ( 0 ){
      // Simulated hits are already contained in sim DST
      SvxSimulator *svxsim = new SvxSimulator();
      svxsim->Verbosity(0);
      se->registerSubsystem(svxsim);
    }
    
    if ( backgroundfile )
      {
	SvxEmbedSimhit *svxembed = new SvxEmbedSimhit();
	svxembed->set_StripixelNoise(0.0); // no noise
	svxembed->Verbosity(0);
	se->registerSubsystem(svxembed);
      }
    
    /*
    SvxDecode *svxdecode = new SvxDecode();
    svxdecode->Verbosity(0);
    svxdecode->includePixel(true);
    svxdecode->includeStripixel(true);
    svxdecode->setAdcOffset(24);
    svxdecode->setAdcCutoff(-24);
    se->registerSubsystem(svxdecode);
    */

    SvxApplyHotDead *svxhotdead = new SvxApplyHotDead();
    svxhotdead->Verbosity(0);
    se->registerSubsystem(svxhotdead);

    SvxReco *svxrec = new SvxReco();
    svxrec->Verbosity(0);
    svxrec->set_ThisIsSimulation();
    svxrec->set_StripixelAdcSumThreshold(0);
    se->registerSubsystem(svxrec);

    SvxPriVertexSeedFinder *svxvtxseedfinder = new SvxPriVertexSeedFinder();
    svxvtxseedfinder->Verbosity(0);
    //se->registerSubsystem(svxvtxseedfinder);

    SvxStandAloneReco *svxstandalone = new SvxStandAloneReco();
    svxstandalone->Verbosity(0);
    svxstandalone->setVertexRecoFlag(2);
		svxstandalone->setPPFlag(false);
    //se->registerSubsystem(svxstandalone);

    SvxPrimVertexFinder *svxprimvtxfinder = new SvxPrimVertexFinder();
    svxprimvtxfinder->Verbosity(0);
    //se->registerSubsystem(svxprimvtxfinder);
  }


  // Mutr/Muid unpacking
	MuonUnpackSim* muon_unpack = new MuonUnpackSim();
	// Treat even MC backgrounds as "real" backgrounds because there isn't a utility
	// to merge MC information. In this case, only the "signal" MC information will
	// show up in the evaluation ntuples:
	if( backgroundfile ) muon_unpack->SetMode( MuonUnpackSim::MC_SIGNAL_REAL_BG );
	else muon_unpack->SetMode( MuonUnpackSim::MC_SIGNAL_NO_BG );

	muon_unpack->SetVtxToUse( use_vertex );
  se->registerSubsystem( muon_unpack );

	// Mutr/Muid reconstruction
	// Note:  MuonDev must be called before FvtxRecoWithMut
	MuiooReco* muioo_reco = new MuiooReco();
	muioo_reco->set_max_occupancy_per_arm(1000);
	se->registerSubsystem( muioo_reco );

	// RPC unpacking/reconstruction
	if( use_RPC )
	  {
	    gSystem->Load("librpc_subsysreco");
	    //if( backgroundfile ) se->registerSubsystem( new RpcUnpackSim( "RPCUNPACKSIM", RpcUnpackSim::MC_SIGNAL_REAL_BG ) );
	    if( backgroundfile ) se->registerSubsystem( new RpcUnpackSim( "RPCUNPACKSIM", RpcUnpackSim::MC_SIGNAL_MC_BG ) );
	    else  se->registerSubsystem( new RpcUnpackSim( "RPCUNPACKSIM", RpcUnpackSim::MC_SIGNAL_NO_BG ) );
	    se->registerSubsystem( new RpcReco() );
	  }

	MuonDev* muon_dev = new MuonDev();
	se->registerSubsystem( muon_dev );

	if( use_FVTX )
	{
	  if( FVTX_mcreco ) {
	    
	    FvtxUnpackSim* fvtx_unpack = new FvtxUnpackSim();
	    if( backgroundfile ) fvtx_unpack->SetMode( FvtxUnpackSim::MC_SIGNAL_MC_BG );
	    else fvtx_unpack->SetMode( FvtxUnpackSim::MC_SIGNAL_NO_BG );
	    se->registerSubsystem( fvtx_unpack );
	    se->registerSubsystem( new FvtxRecoMC() );
	    
	  } else if( FVTX_realreco ) {
	    
	    FvtxUnpackSim* fvtx_unpack = new FvtxUnpackSim();
	    // Treat even MC backgrounds as "real" backgrounds because there isn't a utility
	    // to merge MC information. In this case, only the "signal" MC information will
	    // show up in the evaluation ntuples:
	    if( backgroundfile ) fvtx_unpack->SetMode( FvtxUnpackSim::MC_SIGNAL_REAL_BG );
	    else fvtx_unpack->SetMode( FvtxUnpackSim::MC_SIGNAL_NO_BG );
	    se->registerSubsystem( fvtx_unpack );
	    
	    FvtxReco *fvtx_reco = new FvtxReco();
	    fvtx_reco->set_n_coord_cut(15000);
	    se->registerSubsystem( fvtx_reco );
	    
	    // primary vertex from FVTX
	    // Skip vertex finding in embedding simulation
	    //if ( use_vertex == MuonUnpackSim::RECO ){
	    if ( 0 ){
	      FvtxPrimVertex* fvtxprimvtx = new FvtxPrimVertex();
	      fvtxprimvtx->set_source(FvtxPrimVertex::Tracks);
	      fvtxprimvtx->set_clustering(FvtxPrimVertex::AllInOne); // AllInOne for single vertex, Cesars for multiple verteces
	      se->registerSubsystem(fvtxprimvtx);
	    }
	  }
	  
	  // Perform FVTX-Mutr track matching and refit track:
	  FvtxRecoWithMut *fvtx_reco_withmut = new FvtxRecoWithMut();
	  se->registerSubsystem( fvtx_reco_withmut );
	}
		
	/*
	  global Reco must be run after the reconstruction
    but before the evalutations for all nodes/ntuples to be filled
    properly
  */
  se->registerSubsystem( new GlobalReco() );
  se->registerSubsystem( new GlobalReco_muons() );

  // prdf packer
  if( prdffile && write_merged_prdf )
  se->registerSubsystem( new MuonPackPRDF() );

  PHInclusiveNanoCuts *MWGcuts = new MWGInclusiveNanoCutsv2();
  se->registerSubsystem(new MWGFvtxReco(MWGcuts));
  
  gSystem->Load("libpicodst_object.so");
  mFillSingleMuonContainer* msngl = new mFillSingleMuonContainer();
  msngl->set_lastgap_cut(1);
  msngl->set_is_sim(is_sim); // Select which vertex used for DCA calculation
  se->registerSubsystem(msngl);
  
  // Fill FVTX evaluation ntuples: needed to match real and mc tracks
  if( do_evaluation )
    {
      MuonEval* mueval = new MuonEval();
      mueval->set_flags(0); // Do not make a evaluation file
      se->registerSubsystem(mueval);
      if ( use_FVTX )
	{
	  //FvtxEval* fvtxeval = new FvtxEval("FvtxEval","fvtx_eval_embed_pisa.root");
	  //se->registerSubsystem(fvtxeval);
	  //
	  FvtxMCEval* fvtxeval_mc = new FvtxMCEval("FvtxMCEval","fvtx_mc_eval_embed_pisa.root");
	  se->registerSubsystem(fvtxeval_mc);
	}
    }

  ///////////////////////////////////////////
  // Output manager
  ///////////////////////////////////////////

  // dst output manager
  if( dstfile && write_dst )
  {

    Fun4AllDstOutputManager *dstManager  = new Fun4AllDstOutputManager("DSTOUT",dstfile);
    se->registerOutputManager(dstManager);

    // Header and vertex nodes
    dstManager->AddNode("RunHeader");
    dstManager->AddNode("EventHeader");
    dstManager->AddNode("VtxOut");
    dstManager->AddNode("BbcOut");
    dstManager->AddNode("BbcRaw");
    dstManager->AddNode("ZdcOut");
    dstManager->AddNode("ZdcRaw");
    dstManager->AddNode("TrigLvl1");

    dstManager->AddNode("TMuiPseudoLL1");

    // muioo nodes
    dstManager->AddNode("TMCPrimary");
    dstManager->AddNode("TMuiMCHitO");
    dstManager->AddNode("TMuiHitO");
    dstManager->AddNode("TMuiClusterO");
    dstManager->AddNode("TMui1DRoadO");
    dstManager->AddNode("TMuiRoadO");
    dstManager->AddNode("TMuiPseudoBLTO");
    dstManager->AddNode("TMuiPseudoLL1");

    // mutoo nodes
    dstManager->AddNode("TMutMCHit");
    dstManager->AddNode("TMutMCTrk");
    dstManager->AddNode("TMutHit");
    dstManager->AddNode("TMutClus");
    dstManager->AddNode("TMutCoord");
    dstManager->AddNode("TMutGapCoord");
    dstManager->AddNode("TMutStub");
    dstManager->AddNode("TMutTrk");
    dstManager->AddNode("TMutVtx");

    // rpc nodes
    dstManager->AddNode("TRpcMCHit");
    dstManager->AddNode("TRpcHit");
    dstManager->AddNode("TRpcClus");
    dstManager->AddNode("TRpcCoord");
    dstManager->AddNode("TRpcTrk");

    // Fvtx nodes
    if( use_FVTX )
    {
      dstManager->AddNode( "TFvtxTrk" );
      dstManager->AddNode( "TFvtxCompactTrk" );
      dstManager->AddNode( "TFvtxMCHit" );
      dstManager->AddNode( "TFvtxMCTrk" );
      dstManager->AddNode("SVX");
      dstManager->AddNode("SvxGhitClusterList");
      dstManager->AddNode("SvxGhitList");
    }

    // From EVA node
    dstManager->AddNode("header");
    dstManager->AddNode("fkin");
    dstManager->AddNode("primary");
    dstManager->AddNode("pythia");
    dstManager->AddNode("MCHepMCParticleContainer");

    dstManager->AddNode("SingleMuonContainer");
    dstManager->AddNode("PHMuoTracksOO");
  }

  
  // ndst output manager
  if( ndstfile && write_ndst )
  {

    Fun4AllDstOutputManager *ndstManager = new Fun4AllDstOutputManager( "MWGOUT",ndstfile );
    se->registerOutputManager(ndstManager);

    ndstManager->AddNode("RunHeader");
    ndstManager->AddNode("EventHeader");
    ndstManager->AddNode("header");
    ndstManager->AddNode("BbcOut");
    ndstManager->AddNode("PHGlobal");
    ndstManager->AddNode("PHGlobal_MUON");
    ndstManager->AddNode("PHMuoTracksOO");
    ndstManager->AddNode("TrigLvl1");
    ndstManager->AddNode("TMuiPseudoBLTO");
    ndstManager->AddNode("TMuiPseudoLL1");
    ndstManager->AddNode("VtxOut");
    ndstManager->AddNode("RpSumXYObject");
    ndstManager->AddNode("ReactionPlaneObject");
  }

  // picodst output manager
  if( write_pdst )
    {
      	
      mFvtxMuTrAssociation* mfvtxmutr = new mFvtxMuTrAssociation( );
      se->registerSubsystem(mfvtxmutr);
      mfvtxmutr->set_nsigmas_fvtxmutr_match(9999.9);
      mfvtxmutr->set_chi2_fvtx_cut(20);
      mfvtxmutr->set_chi2_fvtx_prob_cut(0.05);
      mfvtxmutr->set_chi2_fvtxmutr_cut(3);
      mfvtxmutr->set_min_nhits_fvtx(2);
      mfvtxmutr->set_keep_only_best_match( false );
      
      /*
	mFillDiMuonContainer* mdi = new mFillDiMuonContainer(false);
      mdi->set_is_sim(is_sim);
      mdi->set_bbcz_cut(100.0);
      mdi->set_is_pp(false); // Affect centrality
      //mdi->set_mass_cut(1.5);
      se->registerSubsystem(mdi);
      */

      if(fill_mutr_mc_info)
	{
	  if(use_FVTX){
	    mFillMCSingleMuonFvtxContainer* msngl_mc_fvtx = new mFillMCSingleMuonFvtxContainer();
	    msngl_mc_fvtx->SetPythiaNodeName("MCHepMCParticleContainer");
	    se->registerSubsystem(msngl_mc_fvtx);
	  }
	  
	  //********************* Mutr MC information Module ******************//
	  //	  mFillMCDiMuonContainer* mdi_mc = new mFillMCDiMuonContainer();
	  //	  se->registerSubsystem(mdi_mc);
	}
      
      mFillMutrRefitSingleMuonContainer* msngl_MutrRefit = new mFillMutrRefitSingleMuonContainer();
      se->registerSubsystem(msngl_MutrRefit);
     
      Fun4AllOutputManager *outsmu = new Fun4AllDstOutputManager("Outsmu",singlepdstout);
      outsmu->AddNode("SingleMuonContainer");
      outsmu->AddNode("DiMuonContainer");
      outsmu->AddNode("MutrRefitSingleMuonContainer");
      outsmu->AddNode("Sync");
      outsmu->AddNode("VtxOut");
      outsmu->AddNode("PHGlobal");
      outsmu->AddNode("RunHeader");
      outsmu->AddNode("EventHeader");
      outsmu->AddNode("MCSingleMuonFvtxContainer"); // Mutr MC information container
      outsmu->AddNode("MCDiMuonContainer"); // Mutr MC information container
      //      outsmu->AddNode("MCHepMCParticleContainer");
      outsmu->AddEventSelector("mFillSingleMuonContainer");
      se->registerOutputManager(outsmu);
    }
   
  ///////////////////////////////////////////
  // Input manager
  ///////////////////////////////////////////
  Fun4AllDstInputManager *ipythia = new Fun4AllDstInputManager("HEPMC","DST","TOP");
  se->registerInputManager(ipythia);
  se->fileopen(ipythia->Name(), pythiafile);

  rc->set_CharFlag("EMBED_MC_TOPNODE","SIGNAL");
  rc->set_CharFlag("EMBED_REAL_TOPNODE","BACKGROUND");
  Fun4AllInputManager *signal = new Fun4AllNoSyncDstInputManager( "SIGNAL_IM", "DST", "SIGNAL" );
  se->registerInputManager( signal );
  se->fileopen( signal->Name(), signalfile );
  
  // try load simulated background, if requested
  if( backgroundfile )
  {
    Fun4AllInputManager *background = new Fun4AllNoSyncDstInputManager( "BACKGROUND_IM", "DST", "BACKGROUND" );
    se->registerInputManager(background);
    if ( strstr(backgroundfile,"root") ){
      se->fileopen( background->Name(),backgroundfile );
    }else{
      background->AddListFile(backgroundfile);
    }
  }

  /*
    try load background also as a head dst, to get global detectors
    this should not interfere with mutoo nodes (even if duplicated) since they
    are overwritten by the PHMapManager
  */
  if( backgroundfile )
  {
    cout << "Fun4Muons_RecoDST_sim - reoppening BACKGROUND under DST node to get additionnal nodes copied.\n";
    Fun4AllInputManager *background = new Fun4AllNoSyncDstInputManager( "BACKGROUND_DST_IM", "DST" );
    se->registerInputManager(background);
    if ( strstr(backgroundfile,"root") ){
      se->fileopen( background->Name(),backgroundfile );
    }else{
      background->AddListFile(backgroundfile);
    }
  } else if( signalfile ) {
    cout << "Fun4Muons_RecoDST_sim - reoppening SIGNAL under DST node to get additionnal nodes copied.\n";
    Fun4AllInputManager *signal = new Fun4AllNoSyncDstInputManager( "SIGNAL_DST_IM", "DST" );
    se->registerInputManager( signal );
    if ( strstr(signalfile,"root") ){
      se->fileopen( signal->Name(), signalfile );
    }else{
      signal->AddListFile(signalfile);
    }
  }
  
  se->run(nEvents);
  se->End();
  //MuonUtil::dump_process_time();
  cout << "Completed reconstruction." << endl;

}
