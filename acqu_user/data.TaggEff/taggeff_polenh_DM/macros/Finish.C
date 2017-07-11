// Stuff to do at the end of an analysis run.


void FinishBasic(TString name)
{
	TString path;

	cout << "End-of-Run macro executing";
	cout << endl;

	path =  "histograms/";

	path.Append( name);
	path.Append( ".root");
	TFile f( path, "recreate");
	gROOT->GetList()->Write();
	f.Close();

	path.Prepend( "All histograms saved to \"");
	path.Append( "\"");
	cout << path;
	cout << endl;
}

void FinishData( TString exp, TString tgt, TString name)
{
	TString path;

	cout << "End-of-Run macro executing";
	cout << endl;

	path = "histograms/" + exp + "/" + tgt + "/";

	path.Append( name);
	path.Append( ".root");
	TFile f( path, "recreate");
	gROOT->GetList()->Write();
	f.Close();

	path.Prepend( "All histograms saved to \"");
	path.Append( "\"");
	cout << path;
	cout << endl;
}

void FinishMC( Char_t tgt[2], Char_t cond[32], Int_t eg)
{
	Char_t dir[128];

	sprintf( dir, "histograms/MonteCarlo/%s_%s_%d.root", tgt, cond, eg);

	printf( "End-of-Run macro executing\n");

	TFile f( dir, "recreate");
	gROOT->GetList()->Write();
	f.Close();

	printf( "All histograms saved to %s\n", dir);

	gROOT->ProcessLine( ".L ThetaEff.C");

	ThetaEffBatch( eg, tgt, cond);

	printf( "Theta efficiencies calculated for %s %s %d\n", tgt, cond, eg);
}


void FinishTE()
{
	Int_t run;
	TString name;
	sscanf( gAR->GetFileName(), "scratch/TaggEff_%i.dat\n", &run);
	cout<<run<<endl;

	TString name = Form( "histograms/TaggEff_%i.out.root", run);

	cout << "End-of-Run macro executing";
 	cout << endl;

 	TFile f( name, "recreate");
 	gROOT->GetList()->Write();
 	f.Close();

 	name.Prepend( "All histograms saved to \"");
 	name.Append( "\"");
 	cout << name;
 	cout << endl;
}
