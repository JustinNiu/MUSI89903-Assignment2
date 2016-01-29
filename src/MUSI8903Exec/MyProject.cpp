
// standard headers

#include "MUSI8903Config.h"

// project headers
#include "ErrorDef.h"
#include "Ringbuffer.h"

#include "MyProject.h"

static const char*  kCMyProjectBuildDate             = __DATE__;


CMyProject::CMyProject ()
{
    // this never hurts
    this->reset ();
}


CMyProject::~CMyProject ()
{
    this->reset ();
}

const int  CMyProject::getVersion (const Version_t eVersionIdx)
{
    int iVersion = 0;
    
    switch (eVersionIdx)
    {
        case kMajor:
            iVersion    = MUSI8903_VERSION_MAJOR;
            break;
        case kMinor:
            iVersion    = MUSI8903_VERSION_MINOR;
            break;
        case kPatch:
            iVersion    = MUSI8903_VERSION_PATCH;
            break;
        case kNumVersionInts:
            iVersion    = -1;
            break;
    }
    
    return iVersion;
}
const char*  CMyProject::getBuildDate ()
{
    return kCMyProjectBuildDate;
}

Error_t CMyProject::create (CMyProject*& pCMyProject)
{
    pCMyProject = new CMyProject ();
    
    if (!pCMyProject)
        return kUnknownError;
    
    
    return kNoError;
}

Error_t CMyProject::destroy (CMyProject*& pCMyProject)
{
    if (!pCMyProject)
        return kUnknownError;
    
    pCMyProject->reset ();
    
    delete pCMyProject;
    pCMyProject = 0;
    
    return kNoError;
    
}

Error_t CMyProject::init()
{
    // allocate memory
    
    // initialize variables and buffers
    
    return kNoError;
}

Error_t CMyProject::reset ()
{
    // reset buffers and variables to default values
    
    return kNoError;
}