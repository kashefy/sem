/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package model.simulation;

/**
 *
 * @author woodstock
 */
import model.*;
import model.features.*;
import model.encoding.*;
import model.neuron.*;
import model.competition.*;
import model.evaluation.*;
import model.utils.*;
import model.utils.files.*;
import model.attention.*;

import java.io.*;
import model.utils.files.AbstractDataLoader;
import model.utils.files.FileIO;

public class SimulationMNISTInterm extends AbstractSimulation{
               
    // stimuli
    int m_nofRows;
    int m_nofCols;
    int [] m_stimuliDims;
    int m_nofStimElements;
    int m_totalNofStimuli;
    
    // feature maps
    OrientationMap m_mapOrient;
    
    // encoding
    Encoder m_encoder;
    
    // prediction, learning, classification
    int m_nofYNeurons; // get value from enocder
    int m_nofLearners;
    int m_nofLearnersAux;
    ZNeuron [] m_arrZNeurons;
    ZNeuron [] m_arrZNeuronsAux;
    ZNeuron [] m_arrZNeuronsAll;
    
    // competition
    AbstractCompetition m_wta;
    AbstractCompetition m_wtaAll;
    
    AbstractAttention m_attention;
    
    public void setParams(SimulationParams params){
        
        m_params = params;
 
        /*
        m_strMainOutputDir = params.getMainOutputDir();
        m_strMainInputDir = params.getMainInputDir();

        // data
        m_nofTrainStimuli = params.getNofTrainStimuli();
        m_nofTestStimuli = params.getNofTestStimuli();
        m_nofRows = params.getNofRows();
        m_nofCols = params.getNofCols();

        // encoding parameters
        m_encFrequency = params.getEncFrequency();
        m_deltaT = params.get;
        m_encDurationInMilSec = params.get;
        m_popCodeFanOut = params.get;

        // prediction parameters
        m_nofCauses = params.get;
        m_nofLearners = params.get;

        // parameter objects

        m_learnerParams = params.get;

        m_competitionParams = params.get;

        m_featParams = params.get;
         */
    }
    
    public void init(){
        
        // stimuli
        
        m_dataLoader = new DataLoaderImageSetCSV();
        m_dataLoader.setParams(m_params.getMainInputDir());
        m_dataLoader.init();
        m_dataLoader.load();
        
        // complete and correct parameters from data loader
        m_totalNofStimuli  = m_dataLoader.getNofSamples();
        m_params.setNofTrainStimuli((int)(m_params.getNofTrainStimuli()/100.0* m_totalNofStimuli));
        m_params.setNofTestStimuli(m_totalNofStimuli-m_params.getNofTrainStimuli());
        m_params.setNofCauses(m_dataLoader.getNofClasses());
        
        m_stimuliDims = m_dataLoader.getDims();
        m_nofRows     = m_stimuliDims[ FileIO.DIM_INDEX_ROWS ];
        m_params.setNofRows(m_nofRows);
        m_nofCols     = m_stimuliDims[ FileIO.DIM_INDEX_COLS ];
        m_params.setNofCols(m_nofCols);
        
        m_nofStimElements = m_nofRows * m_nofCols;
        
        // feature maps inits       
        m_mapOrient = new OrientationMap(); 
        m_mapOrient.setParams(m_params.getFeatureMapParamsRef());
        m_mapOrient.init();
        
        // encoding inits
        // need to improve init for m_encoder to conform with other inits
        //m_encoder = new Encoder();
        m_encoder = new EncoderSoftMax();
        m_encoder.setFeatureMap(m_mapOrient);
        m_encoder.init(m_params.getEncFrequency(), m_params.getDeltaT(), m_params.getEncDurationInMilSec(), 
                m_nofRows*m_nofCols*m_mapOrient.getNofFeatureSets(), m_params.getPopCodeFanOut());
        m_encoder.setInputDimensions(m_stimuliDims);
        
        m_attention = new AttentionUniform();
        m_attention.init();
        int [] windowDims = m_attention.getWindowDims();
        int nofWindowRows = windowDims[ FileIO.DIM_INDEX_ROWS ];
        int nofWindowCols = windowDims[ FileIO.DIM_INDEX_COLS ];
        
        // predictors/classifiers
        m_nofYNeurons = nofWindowRows*nofWindowCols*m_mapOrient.getNofFeatureSets();//m_encoder.getNofEncoderNodes(); // get value from enocder
        m_params.getLearnerParamsRef().setNofInputNodes(m_nofYNeurons);
        m_nofLearners = m_params.getNofLeaners();
        m_arrZNeurons = new ZNeuron[ m_nofLearners ];       
        
        for(int zi=0; zi<m_nofLearners; zi++){
            
            //arrZNeurons[zi] = new ZNeuron();
            m_arrZNeurons[zi] = new ZNeuronCompact();
            m_arrZNeurons[zi].setParams(m_params.getLearnerParamsRef());
            m_arrZNeurons[zi].init();
        } 
        
        // auxillary learners
        // 0: (predict, fire, update) 100%
        // 1: predict 100%, fire 10%, update 100%
        // 2: predict 100%, fire when no one else fires, update 100%
        // 3: (predict, fire, update) 100% on noise stim only
        m_nofLearnersAux = 0;
        m_arrZNeuronsAux = new ZNeuron[ m_nofLearnersAux ];       
        
        for(int zi=0; zi<m_nofLearnersAux; zi++){
            
            m_arrZNeuronsAux[zi] = new ZNeuronCompact();
            m_arrZNeuronsAux[zi].setParams(m_params.getLearnerParamsRef());
            m_arrZNeuronsAux[zi].init();
        } 
        
        m_arrZNeuronsAll = new ZNeuron[ m_nofLearners+m_nofLearnersAux ];
        int relzi = 0;
        for(int zi=0; zi<m_nofLearners; zi++, relzi++){
            
            m_arrZNeuronsAll[relzi] = m_arrZNeurons[zi];
        } 
        for(int zi=0; zi<m_nofLearnersAux; zi++, relzi++){
            
            m_arrZNeuronsAll[relzi] = m_arrZNeuronsAux[zi];
        }
        
        // competition inits
        //wta = new CompetitionWTAOU();
        m_wta = new WTAPoissonRate();
        m_wta.setParams(m_params.getCompetitionParamsRef());
        m_wta.init();
        m_wta.refToLearners(m_arrZNeurons);
        
        //m_wtaWithAux = new CompetitionWTAOU();
        m_wtaAll = new WTAPoissonRate();
        m_wtaAll.setParams(m_params.getCompetitionParamsRef());
        m_wtaAll.init();
        m_wtaAll.refToLearners(m_arrZNeuronsAll);
        
    }
    
    public void learn(){
        
        // predict - compete - update
        int nofStimuli = m_params.getNofTrainStimuli();
        int gapWidth = m_params.getLearnerParamsRef().getHistoryLength();
        int nofResponsesPerStimulus = m_params.getEncDurationInMilSec()+gapWidth;
        int [][] arrResponse = new int [ nofStimuli ][ nofResponsesPerStimulus ];
        int [] arrResponse1D = new int [ nofStimuli * nofResponsesPerStimulus ];
        
        // activity mask will be reduced to mask weights of nodes with low activity
        int[] arrTrainLabelNames = determineLabelSet(0,nofStimuli-1);
        int nofCauses = m_params.getNofCauses();
        int nofMasks = nofCauses;
        double activityMaskLowerThresholdExcl = 0.05;
        ActivityMask [] arrActivityMask = new ActivityMask[ nofMasks ];
        for(int mi=0; mi<nofMasks; mi++){
            
            arrActivityMask[mi] = new ActivityMask();
            arrActivityMask[mi].setParams(m_nofRows*m_nofCols);
            arrActivityMask[mi].set_lower_threshold_excl(activityMaskLowerThresholdExcl);
            arrActivityMask[mi].init();
        }
        
        int [] allZeroSpikeTrain = new int [ m_nofYNeurons ];
        
        int nofWeightsToWatch = 1;
        double [][][] weightWatch = new double [ m_nofLearners ][ nofWeightsToWatch ][ nofStimuli * nofResponsesPerStimulus ];
        double [][][] rateWatch = new double [ m_nofLearners ][ nofWeightsToWatch ][ nofStimuli * nofResponsesPerStimulus ];
        int [] arrWeightIndicies = new int[ nofWeightsToWatch ];
        arrWeightIndicies[0] = 1500;
        
        // let one aux neuron learn noise
        DataLoaderNoise dataLoaderNoise = new DataLoaderNoise();
        dataLoaderNoise.setParams(m_params.getMainInputDir());
        dataLoaderNoise.init();
        dataLoaderNoise.load();
        
        int [] windowDims = m_attention.getWindowDims();
        int nofWindowRows = windowDims[ FileIO.DIM_INDEX_ROWS ];
        int nofWindowCols = windowDims[ FileIO.DIM_INDEX_COLS ];
        
        for(int si=0; si<nofStimuli; si++){
            
            // transform input into set of spike trains
            int nCurrentLabel = m_dataLoader.getLabel(si);
            double [] stimulus = m_dataLoader.getSample(si);
            
            int [][] spikesY = m_encoder.encode(stimulus);
            int [][] spikesYNoise = m_encoder.encode(dataLoaderNoise.getSample(0));
            
            for(int li=0; li<nofCauses; li++){

                    if(nCurrentLabel == arrTrainLabelNames[li])
                        arrActivityMask[li].addSample(stimulus);
            } 
                            
            m_attention.setScene(stimulus);
            
            for(int ai=0; ai<10; ai++){

                // for each column in all spike trains
                // predict - compete - update
                for(int yt=0; yt<nofResponsesPerStimulus; yt++){

                    // create array for spikes of all y neurons at time yt
                    // traverse through columns of spikesY[][]
                    int [] spikesYAtT;
                    int [] spikesYNoiseAtT;
                    int [] spikesYAtTSub;
                    if(yt < m_params.getEncDurationInMilSec()){

                        spikesYAtT = ModelUtils.extractColumns(spikesY, yt);
                        spikesYNoiseAtT = ModelUtils.extractColumns(spikesYNoise, yt);
                        
                        m_attention.attend(stimulus);
                        int [] windowLoc = m_attention.getWindowLoc();
                        int windowRow = windowLoc[ FileIO.DIM_INDEX_ROWS ];
                        int windowCol = windowLoc[ FileIO.DIM_INDEX_COLS ];

                        spikesYAtTSub = new int[ m_nofYNeurons ];
                        int nofFeatures = m_mapOrient.getNofFeatureSets();

                        int rStart = windowRow;
                        int rEnd = rStart + nofWindowRows;
                        int cStart = windowCol;
                        for(int r=rStart; r<rEnd; r++){

                            int rWindow = r-rStart;

    //                        System.out.println(r);
    //                        System.out.println(nofFeatures*(r*m_nofCols+cStart));
    //                        System.out.println(rWindow);
    //                        System.out.println(nofFeatures*rWindow*nofWindowCols);

                            if(nofFeatures*(r*m_nofCols+cStart)+(nofWindowCols * nofFeatures) > spikesYAtT.length){
                                
                                System.out.println("r"+r);
                                System.out.println(nofFeatures*(r*m_nofCols+cStart));
                            }
                            if(nofFeatures*rWindow*nofWindowCols+(nofWindowCols * nofFeatures) > spikesYAtTSub.length){
                                System.out.println("w"+rWindow);
                                System.out.println(nofFeatures*rWindow*nofWindowCols);
                            }

                            System.arraycopy(spikesYAtT, nofFeatures*(r*m_nofCols+cStart), spikesYAtTSub, nofFeatures*rWindow*nofWindowCols, nofWindowCols * nofFeatures);    

                        }
                    }
                    else{
                        spikesYAtT = allZeroSpikeTrain;
                        spikesYAtTSub = allZeroSpikeTrain;
                        spikesYNoiseAtT = allZeroSpikeTrain;
                    }
                      
                    // predict
                    for(int zi=0; zi<m_nofLearners; zi++){

                        double membranePotential;
                        membranePotential = m_arrZNeurons[zi].predict(spikesYAtTSub);
                    }

                    for(int auxi=0; auxi<m_nofLearnersAux; auxi++){

                        m_arrZNeuronsAux[ auxi ].predict(spikesYAtT);
                        if(auxi==3)
                            m_arrZNeuronsAux[ auxi ].predict(spikesYNoiseAtT);
                    }


                    // let Z's compete before updating
                    int wtaResponse = m_wta.compete();
                    //System.out.println(wtaResponse);

                    if(wtaResponse != AbstractCompetition.WTA_NONE){

                        int [] arrWTAResponse;
                        arrWTAResponse = m_wta.getOutcome();
                        for(int zi=0; zi<m_nofLearners; zi++){

                            m_arrZNeurons[ zi ].letFire(arrWTAResponse[ zi ]==1);
                        }
                    }

                    if(m_nofLearnersAux > 0){

                        m_arrZNeuronsAux[0].letFire(true);
                        m_arrZNeuronsAux[1].letFire(yt % 10 == 0);
                        m_arrZNeuronsAux[2].letFire(wtaResponse == AbstractCompetition.WTA_NONE);
                        m_arrZNeuronsAux[3].letFire(true);
                    }

                    // update
                    for(int zi=0; zi<m_nofLearners; zi++){

                        for(int ww=0; ww<nofWeightsToWatch; ww++){

                            //weightWatch[ zi ][ ww ][ si*nofResponsesPerStimulus + yt ] = m_arrZNeurons[ zi ].getWeights()[ arrWeightIndicies[ww] ];
                            //rateWatch[ zi ][ ww ][ si*nofResponsesPerStimulus + yt ] = m_arrZNeurons[ zi ].getLearningRateEta()[ arrWeightIndicies[ww] ];

                        }

                        m_arrZNeurons[zi].update();               
                    }

                    for(int auxi=0; auxi<m_nofLearnersAux; auxi++){

                        m_arrZNeuronsAux[ auxi ].update();
                    }

                    arrResponse[ si ][ yt ] = wtaResponse;
                    arrResponse1D[ si*nofResponsesPerStimulus+yt ] = wtaResponse;
                }     
            }
        }        

        for(int mi=0; mi<nofMasks; mi++){
            arrActivityMask[mi].calc_activity_intensity();
        }
        String strMaskFile = m_params.getMainOutputDir()+"masksClasses.csv";
        ModelPredictionTest.saveMasks(strMaskFile, arrActivityMask);
        
        int predictionStatWindowSize = 20; // in no. of stimuli
        int nofPredictionStatSamples =  nofStimuli/predictionStatWindowSize;
        double [] arrAvgCondEntropy = new double[ nofPredictionStatSamples ];

        PredictionStats predictionStats = new PredictionStats();
        predictionStats.setParams(m_nofLearners,arrTrainLabelNames);
        predictionStats.init();
            
        int pi = 0;
        for(int si=0; si<nofStimuli-predictionStatWindowSize; si+=predictionStatWindowSize){
                     
            for(int subsi=0; subsi<predictionStatWindowSize; subsi++){
                
                int label = m_dataLoader.getLabel(si+subsi);
                for(int ri=0; ri<nofResponsesPerStimulus; ri++){
                    
                     int response = arrResponse[si+subsi][ri];
                     if(response != AbstractCompetition.WTA_NONE){
                        predictionStats.addResponse(response, label);
                     }
                }
               
            }
            arrAvgCondEntropy[pi] = predictionStats.calcConditionalEntropy();
            predictionStats.reset();
            
            pi++;
            
        }
        FileIO.saveArrayToCSV(arrAvgCondEntropy,1,nofPredictionStatSamples,m_params.getMainOutputDir()+"avgCondEntropy.csv");
        
//        System.out.println();
//        for(int i=0; i<m_nofLearners; i++){
//            
//            System.out.print(m_arrZNeurons[i].getBias()+" ");
//            for(int w=0; w<m_nofYNeurons; w++)
//                System.out.print(m_arrZNeurons[i].getWeights()[w]+" ");
//            System.out.println();
//        }
//        for(int i=0; i<m_nofLearnersAux; i++){
//            
//            System.out.print(m_arrZNeuronsAux[i].getBias()+" ");
//            for(int w=0; w<m_nofYNeurons; w++)
//                System.out.print(m_arrZNeuronsAux[i].getWeights()[w]+" ");
//            System.out.println();
//        }
//        System.out.println();
        
        ModelPredictionTest.saveWeights(m_params.getMainOutputDir()+"weights.csv", m_arrZNeurons);
        ModelPredictionTest.saveBiases(m_params.getMainOutputDir()+"biases.csv", m_arrZNeurons);
        if(m_nofLearnersAux > 0){
            
            ModelPredictionTest.saveWeights(m_params.getMainOutputDir()+"weightsAux.csv", m_arrZNeuronsAux);
            ModelPredictionTest.saveBiases(m_params.getMainOutputDir()+"biasesAux.csv", m_arrZNeuronsAux);
        }

        String strWatchDir = m_params.getMainOutputDir()+File.separator+"watch"+File.separator;
        for(int zi=0; zi<m_nofLearners; zi++){
            
            FileIO.saveArrayToCSV(weightWatch[zi],strWatchDir + "weightWatch"+zi+".csv");
            FileIO.saveArrayToCSV(rateWatch[zi], strWatchDir + "rateWatch"+zi+".csv");
        }

    }
    
    public void intermediate(){
        
        DataLoaderWeightSet weightLoader = new DataLoaderWeightSet();
        weightLoader.setParams(m_params.getMainOutputDir());
        weightLoader.setWeightValueFilename("weights.csv");
        weightLoader.init();
        weightLoader.load();
        DataLoaderWeightSet biasLoader = new DataLoaderWeightSet();
        biasLoader.setParams(m_params.getMainOutputDir());
        biasLoader.setWeightValueFilename("biases.csv");
        biasLoader.init();
        biasLoader.load();
        
        m_arrZNeurons = new ZNeuron[ m_nofLearners ];
        
        for(int zi=0; zi<m_nofLearners; zi++){
            
            m_arrZNeurons[zi] = new ZNeuronCompact();
            m_arrZNeurons[zi].setParams(m_params.getLearnerParamsRef());
            m_arrZNeurons[zi].init();
            m_arrZNeurons[zi].setWeights(weightLoader.getSample(zi));
            m_arrZNeurons[zi].setBias(biasLoader.getSample(zi)[0]);
        } 
        
        weightLoader = new DataLoaderWeightSet();
        weightLoader.setParams(m_params.getMainOutputDir());
        weightLoader.setWeightValueFilename("weightsAux.csv");
        weightLoader.init();
        weightLoader.load();
        biasLoader = new DataLoaderWeightSet();
        biasLoader.setParams(m_params.getMainOutputDir());
        biasLoader.setWeightValueFilename("biasesAux.csv");
        biasLoader.init();
        biasLoader.load();
        
        for(int zi=0; zi<m_nofLearnersAux; zi++){
            
            m_arrZNeuronsAux[zi] = new ZNeuronCompact();
            m_arrZNeuronsAux[zi].setParams(m_params.getLearnerParamsRef());
            m_arrZNeuronsAux[zi].init();
            m_arrZNeuronsAux[zi].setWeights(weightLoader.getSample(zi));
            m_arrZNeuronsAux[zi].setBias(biasLoader.getSample(zi)[0]);
        } 
    }
    
    public void test(){
        
        // combine learners with aux learners
        ZNeuron [] old = m_arrZNeurons;
        m_arrZNeurons = new ZNeuron[ m_nofLearners + m_nofLearnersAux ];
        for(int i=0; i<m_nofLearners; i++){
            
            m_arrZNeurons[i] = old[i];
        }
        int reli = m_nofLearners;
        for(int i=0; i<m_nofLearnersAux; i++){
            
            m_arrZNeurons[reli++] = m_arrZNeuronsAux[i];
        }
        m_nofLearners += m_nofLearnersAux;
        
//        System.out.println();
//        for(int i=0; i<m_nofLearners; i++){
//            
//            System.out.print(m_arrZNeurons[i].getBias()+" ");
//            for(int w=0; w<m_nofYNeurons; w++)
//                System.out.print(m_arrZNeurons[i].getWeights()[w]+" ");
//            System.out.println();
//        }
//        System.out.println();
        
        m_wta.refToLearners(m_arrZNeurons);
        m_wtaAll.refToLearners(m_arrZNeuronsAll);
        
        int starti =  m_params.getNofTrainStimuli();
        int endi = m_totalNofStimuli;
        int nofStimuli = endi-starti+1;
        int gapWidth = m_params.getLearnerParamsRef().getHistoryLength();
        int nofResponsesPerStimulus = m_params.getEncDurationInMilSec()+gapWidth;
        
        int[] arrTestLabelNames = determineLabelSet(starti,endi);
        int nofCauses = m_params.getNofCauses();
        
        int nofMasks = m_nofLearners;
        double activityMaskLowerThresholdExcl = 0.05;
        ActivityMask [] arrActivityMask = new ActivityMask[ nofMasks ];
        for(int mi=0; mi<nofMasks; mi++){
            
            arrActivityMask[mi] = new ActivityMask();
            arrActivityMask[mi].setParams(m_nofRows*m_nofCols);
            arrActivityMask[mi].set_lower_threshold_excl(activityMaskLowerThresholdExcl);
            arrActivityMask[mi].enable_logging(m_params.getMainOutputDir()+"activity"+File.separator, Integer.toString(mi));
            arrActivityMask[mi].init();
        }
        
        // membranePotentialZ : nofZ,nofstimuli,durationOfSpikeTrain Y for each stimulus per Z
        double [][][] membranePotentialZ = new double [ m_nofLearners ][ nofStimuli ][ nofResponsesPerStimulus ] ;
        int [][] arrResponse = new int [ nofStimuli ][ nofResponsesPerStimulus ];
        int noWinnerCount = 0;
        
        PredictionStats predictionStats = new PredictionStats();
        predictionStats.setParams(m_nofLearners,arrTestLabelNames);
        predictionStats.init();

        int [] windowDims = m_attention.getWindowDims();
        int nofWindowRows = windowDims[ FileIO.DIM_INDEX_ROWS ];
        int nofWindowCols = windowDims[ FileIO.DIM_INDEX_COLS ];
        
        int [] allZeroSpikeTrain = new int [ m_nofYNeurons ];
        
        int relSi = 0;
        for(int si=starti; si<endi; si++){
                
            int nCurrentLabel   = m_dataLoader.getLabel(si);
            double [] stimulus  = m_dataLoader.getSample(si);
            int [][] spikesY    = m_encoder.encode(stimulus);
            //int [][] temp = ModelUtils.pad2D(spikesY, 6, ModelUtils.MODE_PAD_COLS);

            m_attention.setScene(stimulus);
            
            for(int ai=0; ai<10; ai++){
                
                for(int yt=0; yt<nofResponsesPerStimulus; yt++){

                    // create array for spikes of all y neurons at time yt
                    // traverse through columns of spikesY[][]                    
                    int [] spikesYAtT;
                    int [] spikesYAtTSub;
                    //spikesYAtT = (yt < m_params.getEncDurationInMilSec())? ModelUtils.extractColumns(spikesY, yt) : allZeroSpikeTrain;
                    if(yt < m_params.getEncDurationInMilSec()){
                        
                        spikesYAtT = ModelUtils.extractColumns(spikesY, yt);
                        
                        m_attention.attend(stimulus);
                        int [] windowLoc = m_attention.getWindowLoc();
                        int windowRow = windowLoc[ FileIO.DIM_INDEX_ROWS ];
                        int windowCol = windowLoc[ FileIO.DIM_INDEX_COLS ];

                        spikesYAtTSub = new int[ m_nofYNeurons ];
                        int nofFeatures = m_mapOrient.getNofFeatureSets();

                        int rStart = windowRow;
                        int rEnd = rStart + nofWindowRows;
                        int cStart = windowCol;
                        for(int r=rStart; r<rEnd; r++){

                            int rWindow = r-rStart;

                            System.arraycopy(spikesYAtT, nofFeatures*(r*m_nofCols+cStart), spikesYAtTSub, nofFeatures*rWindow*nofWindowCols, nofWindowCols * nofFeatures);
                        }
                    }
                    else{
                        
                        spikesYAtT = allZeroSpikeTrain;
                         spikesYAtTSub = allZeroSpikeTrain;
                    }
                    
                    for(int zi=0; zi<m_nofLearners; zi++){

                        membranePotentialZ[zi][ relSi ][ yt ] = m_arrZNeurons[zi].predict(spikesYAtTSub);
                    }

                    int wtaResponse = m_wta.compete();

                    if(wtaResponse != AbstractCompetition.WTA_NONE){

                        if(wtaResponse == AbstractCompetition.WTA_ALL)
                            wtaResponse = PredictionStats.RESPONSE_ALL;
                        predictionStats.addResponse(wtaResponse, nCurrentLabel);

                        arrActivityMask[wtaResponse].addSample(stimulus);
                    }
                    else 
                        noWinnerCount++;

                    arrResponse[ relSi ][ yt ] = wtaResponse;
                }
            }
            relSi++;
        }    
        
        for(int mi=0; mi<nofMasks; mi++){
            arrActivityMask[mi].calc_activity_intensity();
        }
        ModelPredictionTest.saveMasks(m_params.getMainOutputDir()+"masksLearners.csv", arrActivityMask);

        ModelPredictionTest.saveResponses(m_params.getMainOutputDir()+"response.csv", arrResponse);    
                
        System.out.println("test set size: "+m_params.getNofTestStimuli());
        
        // print prediction stats
        int [][] firingCounts = predictionStats.getFiringCounts();
        int [] firingSums = predictionStats.calcFiringSums();
        double [][] firingProbs = predictionStats.calcFiringProbs();
        double [] arrCondEntropy = new double [ m_nofLearners ];
        predictionStats.calcConditionalEntropy(arrCondEntropy);
        
        for(int zi=0; zi<m_nofLearners; zi++){

            for(int ci=0; ci<nofCauses; ci++){

                System.out.print(firingCounts[zi][ci] + "  ");
            }            

            System.out.print("  \u03A3  " + firingSums[zi]);
            System.out.print("  condEntr  " + arrCondEntropy[ zi ]);
            System.out.println();

        }
        System.out.println("No winner: "+noWinnerCount);
        
        ModelPredictionTest.savePredictionStats(m_params.getMainOutputDir()+"predictionStats.csv", firingProbs, arrCondEntropy);
        
        m_nofLearners -= m_nofLearnersAux;
    }
    
    public void run(){
    
        System.out.println("running");
        learn();
        System.out.println("learn()...done");
        //intermediate();
        test();
        System.out.println("test()...done");
    }    
 
}