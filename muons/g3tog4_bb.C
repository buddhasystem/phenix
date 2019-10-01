void g3tog4_bb(
		 int nevents = 1000, int skip = 0,
		 int run,
		 std::string vtxfile = "dst.root",
		 std::string outName = "pythia.root",
		 std::string normName = "normalization.root"
	    )
{

  //gSystem->Load("libjprof.so");
  //prof *Pr = new prof;

  gROOT->LoadMacro("g4libs.C");
  g4libs();
  gSystem->Load("libfun4all.so");	// framework + reco modules
  gSystem->Load("libfvtx_subsysreco.so");
  gSystem->Load("libsimreco.so");
  gSystem->Load("libPHHepMCNode.so");
  gSystem->Load("libPHG3toG4.so");
  gSystem->Load("libpythia8.so");

  gSystem->ListLibraries();

  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(0);
  
  recoConsts *rc = recoConsts::instance();
  if(run > 0) rc->set_IntFlag("RUNNUMBER",run);

  SubsysReco *sync = new SyncSimreco();
  se->registerSubsystem(sync);

  string particles = "13 -13";
  string parents = "521,511,531,541,513,523,533,543, 411,421,431,413,423,433"; // B and D hadrons
  parents += "-521,-511,-531,-541,-513,-523,-533,-543, -411,-421,-431,-413,-423";

  PHPy8ParticleTrigger *pTrig = new PHPy8ParticleTrigger();
  pTrig->Verbosity(0);
  pTrig->AddParticles(particles);
  pTrig->AddParents(parents);
  pTrig->SetPLow(2.0);
  pTrig->SetAbsEtaHighLow(2.5,1.1);
  pTrig->KeepOnlySelParticleStable();
  pTrig->PrintConfig();

  PHG3toG4ActivityTrigger *trig1 = new PHG3toG4ActivityTrigger();
  trig1->AddParticles(particles);
  trig1->AddParticles("13 -13");
  //  trig->SetDoAbsParticlesOnly(); //Turn this on to compare absolute values of the particle ID 
  trig1->AddDetector("MUI");
  trig1->AddDetectorMinHits("MUI",2);
  trig1->AddDetector("MUT");
  trig1->AddDetectorMinHits("MUT",11);
  trig1->AddDetector("SVX");
  trig1->AddDetectorMinHits("SVX",2);
  trig1->PrintConfig();

  string hepmc_node = "PHHepMCGenEvent";
  PHG3toG4 *myG3toG4 = new PHG3toG4("PHG3toG4","Sim3D++.root","geom.root");
  //  myG3toG4->UseVertexText(vtxfile);
  myG3toG4->UseVertexDST("FVTX");
  myG3toG4->SavePassedHepMCEvents(hepmc_node.c_str());
  myG3toG4->SetNormalizationFile(normName);
  //myG3toG4->CheckOverlaps(true);
  //myG3toG4->SetMaxToF("ALL",400);//nsec
  //myG3toG4->SetMaxStep("SVX",0.1);//mm
  //myG3toG4->SetMaxStep("MUT",1.0);//mm
  //myG3toG4->SetMaxStep("MUI",0.5);//mm
  //  myG3toG4->SetTrackEnergyCut(20.);//MeV
  //myG3toG4->SetMinKinEnergy("BBC",20);//MeV
  //myG3toG4->SetMinKinEnergy("MUT",0.1);//MeV
  //myG3toG4->SetMinEnergyDeposit("BBC",.1);//MeV
  //myG3toG4->SetMinEnergyDeposit("MUI",.002);//MeV
  //myG3toG4->SetBField(false);
  myG3toG4->ReportEvery(100);
  myG3toG4->Verbosity(0);
  myG3toG4->SetConfigPythiaFile("phpythia8_bb.cfg");
  myG3toG4->registerPythiaTrigger(pTrig);
  myG3toG4->RegisterTrigger(trig1);
  //  myG3toG4->SetPhysicsList("QGSP_BERT");
  se->registerSubsystem(myG3toG4);

  PHHepMCFilter* hepmc_filter = new PHHepMCFilter();
  hepmc_filter->SetNodeName(hepmc_node);
  hepmc_filter->AddParticles(particles+","+parents);
  hepmc_filter->AddParents(parents);
  hepmc_filter->SaveFirstGenMothers();
  se->registerSubsystem(hepmc_filter);

  mFillMCHepMCParticleContainer* gen_event = new mFillMCHepMCParticleContainer();
  gen_event->SetNodeName(hepmc_node);
  se->registerSubsystem(gen_event); 

  Fun4AllNoSyncDstInputManager *inputMgr = new Fun4AllNoSyncDstInputManager("DSTVertexIn","DST");
  se->registerInputManager(inputMgr);
  se->fileopen(inputMgr->Name(),vtxfile.c_str());
  //  inputMgr->AddFile(vtxfile.c_str());

  Fun4AllDstOutputManager *dst_output_mgr  = new Fun4AllDstOutputManager("HEPMC",outName.c_str());
  dst_output_mgr->AddNode("Sync");
  dst_output_mgr->AddNode("EventHeader");
  dst_output_mgr->AddNode("VtxOut");
  dst_output_mgr->AddNode("MCHepMCParticleContainer");
  se->registerOutputManager(dst_output_mgr);

  // run over all events
  se->skip(skip);
  se->run(nevents);  
  se->End();

}

