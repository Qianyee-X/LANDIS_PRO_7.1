#include <stdio.h>

#include "system1.h"

#include "error.h"

#include "SiteRemovalMask.h"

#include "GlobalVariables.h"

#include "Stand.h"

#include "HarvestEvent.h"

#include "StandHarvester.h"

#include "HarvestRegime.h"

#include "SiteHarvester.h"

#include "FillingStandHarvester.h"

#include "HarvestVolumeFittingRegime.h"

#include "StandIterator.h"



HarvestVolumeFittingRegime::HarvestVolumeFittingRegime() {

    itsState = START;

    itsEntryDecade = 0;

    itsFinalDecade = 0;

    itsRepeatInterval = 0;

    itsTargetCut = 0;

}



HarvestVolumeFittingRegime::~HarvestVolumeFittingRegime() {

}



/* inline */ int HarvestVolumeFittingRegime::isA() {

    return EVENT_Volume_BA_THINING;

}



int HarvestVolumeFittingRegime::conditions() {

    int passed;



    switch (itsState) {



    case PENDING :

        if (currentDecade == itsEntryDecade) {

            passed = 1;
			
			itsState = TOHARVEST;
			send_parameters_to_current(1,-1);

        } else if (currentDecade>itsEntryDecade&&(currentDecade-itsEntryDecade) % itsRepeatInterval == 0) {

            passed = 1;
			
            itsState = TOHARVEST;
			send_parameters_to_current(1,-1);
		}
		else{

			passed = 0;

		}
		
		if(currentDecade>itsEntryDecade){
			for(int i=0;i<total_reentry_event_instances;i++){
				int inteval_reentry = HarvestVolumeFittingRegime_reentry_event_instances[i].itsReentryInteval;
				if((currentDecade-itsEntryDecade-inteval_reentry)%(itsRepeatInterval) == 0 ||(currentDecade-itsEntryDecade)-inteval_reentry == 0){
					
					passed = 1;
					itsState = TOREENTRY;
					send_parameters_to_current(0,i);
				}
			}
		}
        break;



    case DONE :

        passed = 0;

        break;



    default :

        errorSys("Illegal call to HarvestVolumeFittingRegime::conditions.", STOP);

        break;

    }



    return passed;

}



void HarvestVolumeFittingRegime::harvest() {
	
	StandsCut = 0;
    switch (itsState) {

    case TOHARVEST :
        HarvestRegime::harvest();
		itsState = PENDING;

        break;
	case TOREENTRY:
		
		reharvest();
		
		itsStands.clear();
		itsState = PENDING;
		break;

    default :

        errorSys("Illegal call to HarvestVolumeFittingRegime::harvest.", STOP);

        break;

    }

}

void HarvestVolumeFittingRegime::send_parameters_to_current(int flag, int index){
	if(flag == 1){ // initial or repeat
		itsTargetCut =  itsTargetCut_copy;

		targetProportion =  targetProportion_copy;

		TargetVolume =  TargetVolume_copy;

		Mininum_BA =  Mininum_BA_copy; 

		Small0_Large1  = Small0_Large1_copy;
		for(int i=0;i<200;i++){
			speciesOrder[i]=speciesOrder_copy[i];
			flag_plant[i]=flag_plant_copy[i];
			flag_cut[i]=flag_cut_copy[i];
			num_TreePlant[i]=num_TreePlant_copy[i];
		}
		 itsTargetCut = (int) ((float)(managementAreas(getManagementAreaId())->numberOfActiveSites()) * targetProportion);

		 StandsCut = 0;

		 SitesCut = 0;
	}
	else if(flag == 0&& index<total_reentry_event_instances){//re-entry
		itsTargetCut =  HarvestVolumeFittingRegime_reentry_event_instances[index].itsTargetCut;

		targetProportion =  HarvestVolumeFittingRegime_reentry_event_instances[index].targetProportion;

		TargetVolume =  HarvestVolumeFittingRegime_reentry_event_instances[index].TargetVolume;

		Mininum_BA =  HarvestVolumeFittingRegime_reentry_event_instances[index].Mininum_BA; 

		Small0_Large1  = HarvestVolumeFittingRegime_reentry_event_instances[index].Small0_Large1;
		for(int i=0;i<200;i++){
			speciesOrder[i]=HarvestVolumeFittingRegime_reentry_event_instances[index].speciesOrder[i];
			flag_plant[i]=HarvestVolumeFittingRegime_reentry_event_instances[index].flag_plant[i];
			flag_cut[i]=HarvestVolumeFittingRegime_reentry_event_instances[index].flag_cut[i];
			num_TreePlant[i]=HarvestVolumeFittingRegime_reentry_event_instances[index].num_TreePlant[i];
		}
		 itsTargetCut = (int) ((float)(managementAreas(getManagementAreaId())->numberOfActiveSites()) * targetProportion);

		 StandsCut = 0;

		 SitesCut = 0;
	}
}
void HarvestVolumeFittingRegime::copy_initial_parameters(){
	if(1 == 1){ // initial or repeat
		itsTargetCut_copy = itsTargetCut;

		targetProportion_copy = targetProportion;

		TargetVolume_copy = TargetVolume;

		Mininum_BA_copy = Mininum_BA; 

		Small0_Large1_copy = Small0_Large1 ;
		for(int i=0;i<200;i++){
			speciesOrder_copy[i]=speciesOrder[i];
			flag_plant_copy[i]=flag_plant[i];
			flag_cut_copy[i]=flag_cut[i];
			num_TreePlant_copy[i]=num_TreePlant[i];
		}
	}
}

void HarvestVolumeFittingRegime::read(FILE* infile) {

    if (itsState == START) {

        HarvestRegime::read(infile);

        itsState = PENDING;

    } else {

        errorSys("Illegal call to HarvestVolumeFittingRegime::read.", STOP);

    }

}

void HarvestVolumeFittingRegime::readCustomization1(FILE* infile) {

    



    if (fscanc(infile, "%d", &itsEntryDecade)!=1)

        errorSys("Error reading entry decade from harvest section.",STOP);

	//<Add By Qia on Nov 05 2011>

	if (fscanc(infile, "%d", &itsRepeatInterval)!=1)

        errorSys("Error reading reentry from harvest section.",STOP);

	//itsReentryInterval=itsReentryInterval/pCoresites->TimeStep_Harvest;

	//</Add By Qia on  Nov 05 2011>

	//<Add By Qia on April 08 2009>

	//itsEntryDecade=itsEntryDecade/pCoresites->TimeStep_Harvest;

	//</Add By Qia on April 08 2009>

	//<Aug 03 2009>

	 if (fscanc(infile, "%f", &Mininum_BA)!=1)

        errorSys("Error reading TargetVolume from harvest section.",STOP);

	//</Aug 03 2009>

	  if (fscanc(infile, "%d", &Small0_Large1)!=1)

        errorSys("Error reading TargetVolume from harvest section.",STOP);

	  if (fscanc(infile, "%f", &targetProportion)!=1)

        errorSys("Error reading target proportion from harvest section.",STOP);

	 //<Aug 03 2009>

	 if (fscanc(infile, "%f", &TargetVolume)!=1)

        errorSys("Error reading TargetVolume from harvest section.",STOP);

	 itsTargetCut = (int) (managementAreas(getManagementAreaId())->numberOfActiveSites() * targetProportion);

	//</Aug 03 2009>

	//<Add By Qia on April 08 2009>

	//itsFinalDecade=itsFinalDecade/pCoresites->TimeStep_Harvest;

	//</Add By Qia on April 08 2009>

   // if (fscanc(infile, "%d", &itsReentryInterval)!=1)

   //     errorSys("Error reading reentry interval from harvest section.",STOP);

	//<Add By Qia on April 08 2009>

	//itsReentryInterval=itsReentryInterval/pCoresites->TimeStep_Harvest;

	//if(itsReentryInterval<pCoresites->TimeStep_Harvest)

	//	itsReentryInterval=1;

	//</Add By Qia on April 08 2009>

	 if(pCoresites->specNum>200){

		  errorSys("Two many species for harvest.",STOP);

	 }

	 for(int i=0; i<pCoresites->specNum;i++){
		int temp_spec_order;
		 if (fscanc(infile, "%d", &temp_spec_order)!=1)
			errorSys("Error reading species order from harvest section.",STOP);
		speciesOrder[temp_spec_order-1] = i+1;

		 if (fscanc(infile, "%d", &flag_cut[i])!=1)

			errorSys("Error reading from harvest section.",STOP);

		 if (fscanc(infile, "%d", &flag_plant[i])!=1)

			errorSys("Error reading from harvest section.",STOP);

		 if (fscanc(infile, "%d", &num_TreePlant[i])!=1)

			errorSys("Error reading from harvest section.",STOP);

	 }
	 copy_initial_parameters();
	//<Add By Qia on May 29 2012>
	 total_reentry_event_instances = 0;
	if (fscanc(infile, "%d", &total_reentry_event_instances)!=1)

        errorSys("Error reading standard deviation from harvest section.",STOP);
	if(total_reentry_event_instances > 0){
		HarvestVolumeFittingRegime_reentry_event_instances = new HarvestVolumeFittingRegime_reentry_event[total_reentry_event_instances];
	}
	for(int ii=0;ii<total_reentry_event_instances;ii++){
		HarvestVolumeFittingRegime_reentry_event_instances[ii].HarvestVolumeFittingRegime_load_reentry_parameters(infile);
	}
	//</Add By Qia on May 29 2012>
	 //itsTargetCut = (managementAreas(getManagementAreaId())->numberOfStands());

	 itsTargetCut = (int) ((float)(managementAreas(getManagementAreaId())->numberOfActiveSites()) * targetProportion);

	 StandsCut = 0;

	 SitesCut = 0;

}

void HarvestVolumeFittingRegime_reentry_event::HarvestVolumeFittingRegime_load_reentry_parameters(FILE *infile){
	int temp_load_format;

	if (fscanc(infile, "%d", &temp_load_format)!=1)
        errorSys("Error reading entry decade from harvest section.",STOP);
	if (fscanc(infile, "%d", &temp_load_format)!=1)
        errorSys("Error reading entry decade from harvest section.",STOP);
	if (fscanc(infile, "%d", &temp_load_format)!=1)
        errorSys("Error reading entry decade from harvest section.",STOP);
	if (fscanc(infile, "%d", &temp_load_format)!=1)
        errorSys("Error reading entry decade from harvest section.",STOP);
	if (fscanc(infile, "%d", &temp_load_format)!=1)
        errorSys("Error reading entry decade from harvest section.",STOP);

	if (fscanc(infile, "%d", &itsReentryInteval)!=1)

        errorSys("Error reading entry decade from harvest section.",STOP);

	
	int itsRepeatInterval;
	if (fscanc(infile, "%d", &itsRepeatInterval)!=1)

        errorSys("Error reading reentry from harvest section.",STOP);

	
	//<Aug 03 2009>

	 if (fscanc(infile, "%f", &Mininum_BA)!=1)

        errorSys("Error reading TargetVolume from harvest section.",STOP);

	//</Aug 03 2009>

	  if (fscanc(infile, "%d", &Small0_Large1)!=1)

        errorSys("Error reading TargetVolume from harvest section.",STOP);

	  if (fscanc(infile, "%f", &targetProportion)!=1)

        errorSys("Error reading target proportion from harvest section.",STOP);

	 //<Aug 03 2009>

	 if (fscanc(infile, "%f", &TargetVolume)!=1)

        errorSys("Error reading TargetVolume from harvest section.",STOP);

	// itsTargetCut = (int) (managementAreas(getManagementAreaId())->numberOfActiveSites() * targetProportion);

	//</Aug 03 2009>


	 if(pCoresites->specNum>200){

		  errorSys("Two many species for harvest.",STOP);

	 }

	 for(int i=0; i<pCoresites->specNum;i++){
		int temp_spec_order;
		 if (fscanc(infile, "%d", &temp_spec_order)!=1)
			errorSys("Error reading species order from harvest section.",STOP);
		speciesOrder[temp_spec_order-1] = i+1;

		 if (fscanc(infile, "%d", &flag_cut[i])!=1)

			errorSys("Error reading from harvest section.",STOP);

		 if (fscanc(infile, "%d", &flag_plant[i])!=1)

			errorSys("Error reading from harvest section.",STOP);

		 if (fscanc(infile, "%d", &num_TreePlant[i])!=1)

			errorSys("Error reading from harvest section.",STOP);

	 }
}

void HarvestVolumeFittingRegime::readCustomization2(FILE* infile) {

    setDuration(1);

}



int HarvestVolumeFittingRegime::isHarvestDone() {

    int result;

	if(SitesCut >= itsTargetCut){

		SitesCut = 0;

		return 1;

	}

	else{

		return 0;

	}

    result = SitesCut >= itsTargetCut;

	SitesCut = 0;

    return result;

}

//<Add By Qia on May 31 2011>

double HarvestVolumeFittingRegime::GetBAinACell(int i, int j){

	int m, k;

	double TmpBasalAreaS=0;
	LANDUNIT *l;
	l=pCoresites->locateLanduPt(i,j);
	for(k=1;k<=pCoresites->specNum;k++){

			for(m=1;m<=(*pCoresites)(i,j)->specAtt(k)->longevity/pCoresites->TimeStep;m++)

				{

					TmpBasalAreaS+=pCoresites->GetGrowthRates(k,m,l->ltID)*pCoresites->GetGrowthRates(k,m,l->ltID)/4*3.1415926*(*pCoresites)(i,j)->SpecieIndex(k)->getTreeNum(m,k)/10000.00;

				//TmpBasalAreaS+=0.005454*sites.GetGrowthRates(k,m)*sites.GetGrowthRates(k,m)*sites(i,j)->SpecieIndex(k)->getTreeNum(m,k);

				}

		}

	return TmpBasalAreaS;

}



double HarvestVolumeFittingRegime::CutShareBAinACell_LifeSpanPercent(int i, int j,double target){

	float LifeSpanpercentage[6];

	LifeSpanpercentage[0]= 0.0;

	LifeSpanpercentage[1]= 0.2;

	LifeSpanpercentage[2]= 0.4;

	LifeSpanpercentage[3]= 0.6;

	LifeSpanpercentage[4]= 0.8;

	LifeSpanpercentage[5]= 1.0;

	int *AgeCohortGroup = new int[pCoresites->specNum*6];

	int *AgeArraySmall,*AgeArrayLarge;

	int age_largest = 0;

	int i_count;

	int m, k, m_count,flag_cut_anyspecie=0,flag_plant_any=0;

	double TmpBasalAreaS=0,tempBA,BAcutSpecie;

	int treeNum_save;

	int treeNum_original;

	double BA_actual_cut_cell = 0;
	LANDUNIT *l;
	l=pCoresites->locateLanduPt(i,j);

	for(k=0;k<pCoresites->specNum;k++){

		if (flag_plant[k] == 0){
			flag_plant_any = 1;
			(*pCoresites)(i, j)->SpecieIndex(k + 1)->flag_herb = 0;
		}
		else if (flag_plant[k] == 1){
			(*pCoresites)(i, j)->SpecieIndex(k + 1)->flag_herb = 0;
		}
		else if (flag_plant[k] == 2){
			(*pCoresites)(i, j)->SpecieIndex(k + 1)->flag_herb = 1;
		}

		
		int temp = (*pCoresites)(i,j)->specAtt(speciesOrder[k])->longevity/pCoresites->TimeStep;

		if(age_largest<temp){

			age_largest = temp;

		}

		//AgeCohortGroup[(speciesOrder[k]-1)*6 + 0] = 1;

		//AgeCohortGroup[(speciesOrder[k]-1)*6 + 0] = temp;

		for(i_count=0;i_count<6;i_count++){

			AgeCohortGroup[(speciesOrder[k]-1)*6 + i_count] = temp*LifeSpanpercentage[i_count];

		}

	}

	AgeArraySmall = new int[5*pCoresites->specNum*(age_largest/5+1)];

	AgeArrayLarge = new int[5*pCoresites->specNum*(age_largest/5+1)];

	for(i_count=0;i_count<5;i_count++){

		for(k=0;k<pCoresites->specNum;k++){

			int temp = (*pCoresites)(i,j)->specAtt(speciesOrder[k])->longevity/pCoresites->TimeStep;

			int tempstart = AgeCohortGroup[(speciesOrder[k]-1)*6 + i_count]+1;

			int tempend = AgeCohortGroup[(speciesOrder[k]-1)*6 + i_count+1];

			int value = tempstart;

			for(int tempcount = 0; tempcount<age_largest/5+1 ;tempcount++){

				int pos = i_count*(pCoresites->specNum*(age_largest/5+1))+(speciesOrder[k]-1)*(age_largest/5+1)+tempcount;

				if(value<=tempend){

					AgeArraySmall[pos] = value++;

				}

				else{

					AgeArraySmall[pos] = 0;

				}

			}

			tempstart = AgeCohortGroup[(speciesOrder[k]-1)*6 + i_count+1];

			tempend = AgeCohortGroup[(speciesOrder[k]-1)*6 + i_count]+1;

			value = tempstart;

			for(int tempcount = 0; tempcount<age_largest/5+1 ;tempcount++){

				int pos = i_count*(pCoresites->specNum*(age_largest/5+1))+(speciesOrder[k]-1)*(age_largest/5+1)+tempcount;

				if(value>=tempend){

					AgeArrayLarge[pos] = value--;

				}

				else{

					AgeArrayLarge[pos] = 0;

				}

			}

		}

	}

	if(Small0_Large1 == 0){

		for(i_count=0;i_count<5;i_count++){

			for(int tempcount = 0; tempcount<age_largest/5+1 ;tempcount++){

				for(k=0;k<pCoresites->specNum;k++){

					int pos = i_count*(pCoresites->specNum*(age_largest/5+1))+(speciesOrder[k]-1)*(age_largest/5+1)+tempcount;

					if(AgeArraySmall[pos]>0)

					{

						m= AgeArraySmall[pos];

						if(flag_cut[speciesOrder[k]-1]){

							flag_cut_anyspecie = 1;

							if(TmpBasalAreaS < target){

								tempBA = pCoresites->GetGrowthRates(speciesOrder[k],m,l->ltID)*pCoresites->GetGrowthRates(speciesOrder[k],m,l->ltID)/4*3.1415926*(*pCoresites)(i,j)->SpecieIndex(speciesOrder[k])->getTreeNum(m,speciesOrder[k])/10000.00;

								if(tempBA<=target-TmpBasalAreaS){

									treeNum_original = (*pCoresites)(i,j)->SpecieIndex(speciesOrder[k])->getTreeNum(m,speciesOrder[k]);

									if ((*pCoresites)(i, j)->specAtt(speciesOrder[k])->MinSproutAge <= m*pCoresites->TimeStep && (*pCoresites)(i, j)->specAtt(speciesOrder[k])->maxSproutAge >= m*pCoresites->TimeStep && flag_plant[speciesOrder[k] - 1] != 2){

										(*pCoresites)(i,j)->SpecieIndex(speciesOrder[k])->TreesFromVeg+=treeNum_original;

									}

									(*pCoresites)(i,j)->SpecieIndex(speciesOrder[k])->setTreeNum(m,speciesOrder[k],0);

									TmpBasalAreaS+=tempBA;

									pHarvestsites->AddMoreValueHarvestBA_spec(i,j,speciesOrder[k]-1,tempBA);

									pHarvestsites->AddMoreValueHarvestBA(i,j,tempBA);

								}

								else{

									treeNum_save = (*pCoresites)(i,j)->SpecieIndex(speciesOrder[k])->getTreeNum(m,speciesOrder[k])*(1-(target-TmpBasalAreaS)/tempBA);

									if(treeNum_save>0){

										treeNum_original = (*pCoresites)(i,j)->SpecieIndex(speciesOrder[k])->getTreeNum(m,speciesOrder[k]);

										if ((*pCoresites)(i, j)->specAtt(speciesOrder[k])->MinSproutAge <= m*pCoresites->TimeStep && (*pCoresites)(i, j)->specAtt(speciesOrder[k])->maxSproutAge >= m*pCoresites->TimeStep && flag_plant[speciesOrder[k] - 1] != 2){

											(*pCoresites)(i,j)->SpecieIndex(speciesOrder[k])->TreesFromVeg+=treeNum_original-treeNum_save;

										}

										(*pCoresites)(i,j)->SpecieIndex(speciesOrder[k])->setTreeNum(m,speciesOrder[k],treeNum_save);

										TmpBasalAreaS+=(target-TmpBasalAreaS);

										pHarvestsites->AddMoreValueHarvestBA_spec(i,j,speciesOrder[k]-1,(target-TmpBasalAreaS));

										pHarvestsites->AddMoreValueHarvestBA(i,j,(target-TmpBasalAreaS));

									}

									else{

										treeNum_original = (*pCoresites)(i,j)->SpecieIndex(speciesOrder[k])->getTreeNum(m,speciesOrder[k]);

										if ((*pCoresites)(i, j)->specAtt(speciesOrder[k])->MinSproutAge <= m*pCoresites->TimeStep && (*pCoresites)(i, j)->specAtt(speciesOrder[k])->maxSproutAge >= m*pCoresites->TimeStep && flag_plant[speciesOrder[k] - 1] != 2){

											(*pCoresites)(i,j)->SpecieIndex(speciesOrder[k])->TreesFromVeg+=treeNum_original-treeNum_save;

										}

										(*pCoresites)(i,j)->SpecieIndex(speciesOrder[k])->setTreeNum(m,speciesOrder[k],0);

										TmpBasalAreaS+=tempBA;

										pHarvestsites->AddMoreValueHarvestBA_spec(i,j,speciesOrder[k]-1,tempBA);

										pHarvestsites->AddMoreValueHarvestBA(i,j,tempBA);

									}

								}

							}

							else{

								for(k=0;k<pCoresites->specNum;k++){

									if(flag_plant[speciesOrder[k]-1] == 1){

										int tree_left = (*pCoresites)(i,j)->SpecieIndex(speciesOrder[k])->getTreeNum(1,speciesOrder[k]);

										(*pCoresites)(i,j)->SpecieIndex(speciesOrder[k])->setTreeNum(1,speciesOrder[k],num_TreePlant[speciesOrder[k]-1]+tree_left);

									}

								}

								delete AgeCohortGroup;

								delete AgeArraySmall;

								delete AgeArrayLarge;

								return TmpBasalAreaS;

							}

						}

					}

				}

			}

		}

	}

	if(Small0_Large1 == 1){

		for(i_count=4;i_count>=0;i_count--){

			for(int tempcount = 0; tempcount<age_largest/5+1 ;tempcount++){

				for(k=0;k<pCoresites->specNum;k++){

					int pos = i_count*(pCoresites->specNum*(age_largest/5+1))+(speciesOrder[k]-1)*(age_largest/5+1)+tempcount;

					if(AgeArrayLarge[pos]>0)

					{

						m= AgeArrayLarge[pos];

						if(flag_cut[speciesOrder[k]-1]){

							flag_cut_anyspecie = 1;

							if(TmpBasalAreaS < target){

								tempBA = pCoresites->GetGrowthRates(speciesOrder[k],m,l->ltID)*pCoresites->GetGrowthRates(speciesOrder[k],m,l->ltID)/4*3.1415926*(*pCoresites)(i,j)->SpecieIndex(speciesOrder[k])->getTreeNum(m,speciesOrder[k])/10000.00;

								if(tempBA<=target-TmpBasalAreaS){

									treeNum_original = (*pCoresites)(i,j)->SpecieIndex(speciesOrder[k])->getTreeNum(m,speciesOrder[k]);

									if ((*pCoresites)(i, j)->specAtt(speciesOrder[k])->MinSproutAge <= m*pCoresites->TimeStep && (*pCoresites)(i, j)->specAtt(speciesOrder[k])->maxSproutAge >= m*pCoresites->TimeStep && flag_plant[speciesOrder[k] - 1] != 2){

										(*pCoresites)(i,j)->SpecieIndex(speciesOrder[k])->TreesFromVeg+=treeNum_original;

									}

									(*pCoresites)(i,j)->SpecieIndex(speciesOrder[k])->setTreeNum(m,speciesOrder[k],0);

									TmpBasalAreaS+=tempBA;

									pHarvestsites->AddMoreValueHarvestBA_spec(i,j,speciesOrder[k]-1,tempBA);

									pHarvestsites->AddMoreValueHarvestBA(i,j,tempBA);

								}

								else{

									treeNum_save = (*pCoresites)(i,j)->SpecieIndex(speciesOrder[k])->getTreeNum(m,speciesOrder[k])*(1-(target-TmpBasalAreaS)/tempBA);

									if(treeNum_save>0){

										treeNum_original = (*pCoresites)(i,j)->SpecieIndex(speciesOrder[k])->getTreeNum(m,speciesOrder[k]);

										if ((*pCoresites)(i, j)->specAtt(speciesOrder[k])->MinSproutAge <= m*pCoresites->TimeStep && (*pCoresites)(i, j)->specAtt(speciesOrder[k])->maxSproutAge >= m*pCoresites->TimeStep && flag_plant[speciesOrder[k] - 1] != 2){

											(*pCoresites)(i,j)->SpecieIndex(speciesOrder[k])->TreesFromVeg+=treeNum_original-treeNum_save;

										}

										(*pCoresites)(i,j)->SpecieIndex(speciesOrder[k])->setTreeNum(m,speciesOrder[k],treeNum_save);

										TmpBasalAreaS+=(target-TmpBasalAreaS);

										pHarvestsites->AddMoreValueHarvestBA_spec(i,j,speciesOrder[k]-1,(target-TmpBasalAreaS));

										pHarvestsites->AddMoreValueHarvestBA(i,j,(target-TmpBasalAreaS));

									}

									else{

										treeNum_original = (*pCoresites)(i,j)->SpecieIndex(speciesOrder[k])->getTreeNum(m,speciesOrder[k]);

										if ((*pCoresites)(i, j)->specAtt(speciesOrder[k])->MinSproutAge <= m*pCoresites->TimeStep && (*pCoresites)(i, j)->specAtt(speciesOrder[k])->maxSproutAge >= m*pCoresites->TimeStep && flag_plant[speciesOrder[k] - 1] != 2){

											(*pCoresites)(i,j)->SpecieIndex(speciesOrder[k])->TreesFromVeg+=treeNum_original-treeNum_save;

										}

										(*pCoresites)(i,j)->SpecieIndex(speciesOrder[k])->setTreeNum(m,speciesOrder[k],0);

										TmpBasalAreaS+=tempBA;

										pHarvestsites->AddMoreValueHarvestBA_spec(i,j,speciesOrder[k]-1,tempBA);

										pHarvestsites->AddMoreValueHarvestBA(i,j,tempBA);

									}

								}

							}

							else{

								for(k=0;k<pCoresites->specNum;k++){

									if(flag_plant[speciesOrder[k]-1] == 1){

										int tree_left = (*pCoresites)(i,j)->SpecieIndex(speciesOrder[k])->getTreeNum(1,speciesOrder[k]);

										(*pCoresites)(i,j)->SpecieIndex(speciesOrder[k])->setTreeNum(1,speciesOrder[k],num_TreePlant[speciesOrder[k]-1]+tree_left);

									}

								}

								delete AgeCohortGroup;

								delete AgeArraySmall;

								delete AgeArrayLarge;

								return TmpBasalAreaS;

							}

						}

					}

				}

			}

		}

	}

	
	if(flag_cut_anyspecie || flag_plant_any){

		for(k=0;k<pCoresites->specNum;k++){

			if(flag_plant[speciesOrder[k]-1] == 1){

				int tree_left = (*pCoresites)(i,j)->SpecieIndex(speciesOrder[k])->getTreeNum(1,speciesOrder[k]);

				(*pCoresites)(i,j)->SpecieIndex(speciesOrder[k])->setTreeNum(1,speciesOrder[k],num_TreePlant[speciesOrder[k]-1]+tree_left);

				tree_left = (*pCoresites)(i, j)->SpecieIndex(speciesOrder[k])->getTreeNum(1, speciesOrder[k]);

			}

		}

	}

	delete AgeCohortGroup;

	delete AgeArraySmall;

	delete AgeArrayLarge;

	return TmpBasalAreaS;

}

void HarvestVolumeFittingRegime::reharvest() {
	Stand* stand;
	for (int ii=0; ii<itsStands.size(); ii++) {

//        stand = stands(it.current());  Modified by Vera

		stand = (*pstands)(itsStands.at(ii));
		SitesCut+=stand->numberOfActiveSites();

	//printf("sitesinstand:%d SitesCut:%d Target:%d\n",stand->numberOfActiveSites(),SitesCut,itsTargetCut);

    LDPOINT pt;

	int m, k,i,j;

	double TmpBasalAreaS;

	double TmpBasalAreaS_avg;

	double BA_toCut;

	double shareCut_ACell;

	TmpBasalAreaS = 0;

    for (StandIterator it(*stand); it.moreSites(); it.gotoNextSite()) {

        pt = it.getCurrentSite();

		i = pt.y;

		j = pt.x;

		TmpBasalAreaS += GetBAinACell(i,j);

       //     sumCut += getSiteHarvester()->harvest(pt);

         //   if (getPath()) getPath()->append(pt);

    }

	TmpBasalAreaS_avg = TmpBasalAreaS / pCoresites->CellSize/pCoresites->CellSize/stand->numberOfActiveSites()*10000;

		if(TmpBasalAreaS_avg<=Mininum_BA){

		}

		else{//Cut trees here

			//printf("Enough to Harvest\n");

			StandsCut++;
			//itsStands.append(stand->getId()); //Add By Qia on June 01 2012
			

			BA_toCut = TmpBasalAreaS_avg - TargetVolume;

			BA_toCut = BA_toCut * pCoresites->CellSize * pCoresites->CellSize * stand->numberOfActiveSites()/10000;

			for (StandIterator it(*stand); it.moreSites(); it.gotoNextSite()) {

				 pt = it.getCurrentSite();

				i = pt.y;

				j = pt.x;

				shareCut_ACell = GetBAinACell(i,j)/TmpBasalAreaS*BA_toCut;

				if(shareCut_ACell>0.0){

					//CutShareBAinACell(i,j,shareCut_ACell);

					//SitesCut++;

					CutShareBAinACell_LifeSpanPercent(i,j,shareCut_ACell);

					(*pHarvestsites)(i,j)->harvestType = getUserInputId();

					(*pHarvestsites)(i,j)->lastHarvest =  currentDecade;

				}

				else{

					pHarvestsites->SetValueHarvestBA(i,j,shareCut_ACell);

				}

			}

		}

    }

}
int HarvestVolumeFittingRegime::harvestStand(Stand* stand) {

    /*SiteHarvester theSiteHarvester(getSequentialId(), getRemovalMask(), getReport(), getDuration());

    FillingStandHarvester theStandHarvester(stand, &theSiteHarvester);*/

	

	/*if(StandsCut>=(int)((float)itsTargetCut*targetProportion)){

		return StandsCut;

	}*/

	//stand->itsActiveSites();

	SitesCut+=stand->numberOfActiveSites();

	//printf("sitesinstand:%d SitesCut:%d Target:%d\n",stand->numberOfActiveSites(),SitesCut,itsTargetCut);

    LDPOINT pt;

	int m, k,i,j;

	double TmpBasalAreaS;

	double TmpBasalAreaS_avg;

	double BA_toCut;

	double shareCut_ACell;

	TmpBasalAreaS = 0;


    for (StandIterator it(*stand); it.moreSites(); it.gotoNextSite()) {

        pt = it.getCurrentSite();

		i = pt.y;

		j = pt.x;

		TmpBasalAreaS += GetBAinACell(i,j);

       //     sumCut += getSiteHarvester()->harvest(pt);

         //   if (getPath()) getPath()->append(pt);

    }

	TmpBasalAreaS_avg = TmpBasalAreaS / pCoresites->CellSize/pCoresites->CellSize/stand->numberOfActiveSites()*10000;

		if(TmpBasalAreaS_avg<=Mininum_BA){

		}

		else{//Cut trees here

			//printf("Enough to Harvest\n");

			StandsCut++;
			itsStands.push_back(stand->getId()); //Add By Qia on June 01 2012
			

			BA_toCut = TmpBasalAreaS_avg - TargetVolume;

			if (BA_toCut < 0.0) BA_toCut = TmpBasalAreaS_avg;

			BA_toCut = BA_toCut * pCoresites->CellSize * pCoresites->CellSize * stand->numberOfActiveSites()/10000;


			for (StandIterator it(*stand); it.moreSites(); it.gotoNextSite()) {

				 pt = it.getCurrentSite();

				i = pt.y;

				j = pt.x;

				shareCut_ACell = GetBAinACell(i,j)/TmpBasalAreaS*BA_toCut;

				if(shareCut_ACell>0.0){

					//CutShareBAinACell(i,j,shareCut_ACell);

					//SitesCut++;

					double shareCut = 0.0;

					shareCut = CutShareBAinACell_LifeSpanPercent(i,j,shareCut_ACell);

					(*pHarvestsites)(i,j)->harvestType = getUserInputId();

					(*pHarvestsites)(i,j)->lastHarvest =  currentDecade;

				}

				else{

					pHarvestsites->SetValueHarvestBA(i,j,shareCut_ACell);

				}

			}

		}

    //return StandsCut;

	return 1;

}



//</Add By Qia on May 31 2011>