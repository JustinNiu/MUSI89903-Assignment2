
#include <iostream>
#include <ctime>
#include <cmath>

#include "MUSI8903Config.h"
#include "AudioFileIf.h"
#include "vibrato.h"


using namespace std;

void    showClInfo ();

/////////////////////////////////////////////////////////////////////////////////
// main function
int main(int argc, char* argv[])
{
    std::string             sInputFilePath,                 //!< file paths
                            sOutputFilePath;

    static const int        kBlockSize          = 1024;

    clock_t                 time                = 0;

    float                   **ppfAudioData		= 0,
                            **ppfAudioOutput    = 0,
                            modFreq				= 0.F,
                            modAmpInSecs		= 0.F;
//                            widthInSecs         = 0.1F;

    CAudioFileIf            *phAudioFile        = 0;
    CVibrato				*cvibrato			= 0;
    std::fstream            hOutputFile,
                            hInfile;
    CAudioFileIf::FileSpec_t stFileSpec;

    showClInfo ();

    //////////////////////////////////////////////////////////////////////////////
    // parse command line arguments
    if (argc < 2)
    {
        return -1;
    }
    else
    {
        sInputFilePath  = argv[1];
        sOutputFilePath = sInputFilePath + ".txt";
        modFreq = stof(argv[2]);
        modAmpInSecs = stof(argv[3]);
    }

    //////////////////////////////////////////////////////////////////////////////
    // open the input wave file
    CAudioFileIf::create(phAudioFile);
    phAudioFile->openFile(sInputFilePath, CAudioFileIf::kFileRead);
    if (!phAudioFile->isOpen())
    {
        cout << "Wave file open error!";
        return -1;
    }
    phAudioFile->getFileSpec(stFileSpec);

    //////////////////////////////////////////////////////////////////////////////
    // open the output text file
    hOutputFile.open (sOutputFilePath.c_str(), std::ios::out);
    if (!hOutputFile.is_open())
    {
        cout << "Text file open error!";
        return -1;
    }

    //////////////////////////////////////////////////////////////////////////////
    // allocate memory
    ppfAudioData            = new float* [stFileSpec.iNumChannels];
    ppfAudioOutput          = new float* [stFileSpec.iNumChannels];

    for (int i = 0; i < stFileSpec.iNumChannels; i++)
    {
        ppfAudioData[i] = new float [kBlockSize];
        ppfAudioOutput[i] = new float [kBlockSize];
    }
    time                    = clock();
    //////////////////////////////////////////////////////////////////////////////
    // add vabrito and write it to the output file
    
//    error_check = CVibrato::create(cvibrato, stFileSpec.fSampleRateInHz, stFileSpec.iNumChannels);
//    error_check = cvibrato->init(modFreq, widthInSecs, modAmpInSecs);
//    
   
    
    
    while (!phAudioFile->isEof())
    {
        long long iNumFrames = kBlockSize;
        phAudioFile->readData(ppfAudioData, iNumFrames);
        cvibrato->process(ppfAudioData, ppfAudioOutput, iNumFrames);
        for (int i = 0; i < iNumFrames; i++)
        {
            for (int c = 0; c < stFileSpec.iNumChannels; c++)
            {
                hOutputFile << ppfAudioData[c][i] << "\t";
                hInfile << ppfAudioData[c][i] << " ";
            }
            hOutputFile <<endl;
            hInfile << endl;
        }
        
    }

    cout << "reading/writing done in: \t"    << (clock()-time)*1.F/CLOCKS_PER_SEC << " seconds." << endl;

    //////////////////////////////////////////////////////////////////////////////
    // clean-up
    CAudioFileIf::destroy(phAudioFile);
    hOutputFile.close();
    hInfile.close();

    for (int i = 0; i < stFileSpec.iNumChannels; i++)
    {
        delete [] ppfAudioData[i];
    }
    delete [] ppfAudioData;
    ppfAudioData = 0;
    return 0;
    
    for (int i = 0; i < stFileSpec.iNumChannels; i++)
    {
        delete [] ppfAudioOutput[i];
    }
    delete [] ppfAudioOutput;
    ppfAudioOutput = 0;
    return 0;
    
}


void     showClInfo()
{
    cout << "GTCMT MUSI8903" << endl;
    cout << "(c) 2016 by Hua and Justin" << endl;
    cout  << endl;

    return;
}

