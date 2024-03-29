// aaOcean v2.5 header file
// Author: Amaan Akram 
// www.amaanakram.com
// aaOcean is free software and can be redistributed and modified under the terms of the 
// GNU General Public License (Version 3) as provided by the Free Software Foundation.
// GNU General Public License http://www.gnu.org/licenses/gpl.html

#ifndef AAOCEANCLASS_H
#define AAOCEANCLASS_H

#include "kissfft/kiss_fft.h"
#include "kissfft/tools/kiss_fftnd.h"

class aaOcean
{ 
public:
    aaOcean();
    aaOcean(const aaOcean &cpy);
    ~aaOcean();

    enum arrayType
    {
       eHEIGHTFIELD,
       eCHOPX,
       eCHOPZ,
       eFOAM,
       eEIGENPLUSX,
       eEIGENPLUSZ,
       eEIGENMINUSX,
       eEIGENMINUSZ
    };

    void input( int     resolution,
                unsigned int  seed, 
                float   oceanScale,
                float   oceanDepth,
                float   surfaceTension,
                float   velocity, 
                float   cutoff, 
                float   windDir, 
                int     windAlign, 
                float   damp,  
                float   waveSpeed, 
                float   waveHeight,
                float   chopAmount,
                float   time,
                float   repeatTime,
                bool    doFoam);

    float getOceanData(float uCoord, float vCoord, aaOcean::arrayType type) const;
    void getOceanArray(float *&outArray, aaOcean::arrayType type);
    void clearResidualArrays();

    bool isValid();
    bool isChoppy();
    int getResolution();
    char* getState();
    void getFoamBounds(float& outBoundsMin, float& outBoundsMax);

//private:
    int     m_resolution;
    unsigned int m_seed;
    int     m_windAlign;
    float   m_velocity;
    float   m_windDir;
    float   m_cutoff;
    float   m_damp;
    float   m_oceanScale;
    float   m_oceanDepth;
    float   m_surfaceTension;
    float   m_chopAmount;
    float   m_waveHeight;
    float   m_waveSpeed;
    float   m_time;
    float   m_loopTime;
    float   m_foamBoundmax; //for holding min/max foam
    float   m_foamBoundrange; //for holding min/max foam
    
    //ocean array pointers
    int     *m_xCoord;
    int     *m_zCoord;
    float   *m_hokReal;
    float   *m_hokImag;
    float   *m_hktReal;
    float   *m_hktImag;
    float   *m_kX;
    float   *m_kZ;
    float   *m_omega;
    float   *m_rand1;
    float   *m_rand2;

    //ocean output array pointers
    float *m_out_fft_htField; // y displacement
    float *m_out_fft_chopX;   // x displacement
    float *m_out_fft_chopZ;   // z displacement
    float *m_out_fft_jxxX; // eigenvector X component
    float *m_out_fft_jxxZ; // eigenvector Z component
    float *m_out_fft_jzzX; // eigenvector X component
    float *m_out_fft_jzzZ; // eigenvector Z component
    float *m_out_fft_jxz;  // eigenvalue

    //bool types for various checks during run-time
    bool    m_isAllocated;
    bool    m_isFoamAllocated;
    bool    m_doHoK;
    bool    m_doSetup;
    bool    m_doChop;
    bool    m_doFoam;

    // memory tracking
    int     m_memory;

    kiss_fft_cpx *m_fft_htField;
    kiss_fft_cpx *m_fft_chopX;
    kiss_fft_cpx *m_fft_chopZ;
    kiss_fft_cpx *m_fft_jxx; // eigenvector
    kiss_fft_cpx *m_fft_jzz; // eigenvector
    kiss_fft_cpx *m_fft_jxz; // eigenvalue

    kiss_fftnd_cfg m_planHeightField;
    kiss_fftnd_cfg m_planChopX;
    kiss_fftnd_cfg m_planChopZ;
    kiss_fftnd_cfg m_planJxx;
    kiss_fftnd_cfg m_planJxz;
    kiss_fftnd_cfg m_planJzz;

    char m_state[512]; // array for holding the current state of aaOcean object
    
    // memory management functions
    void allocateBaseArrays();
    void allocateFoamArrays();
    void clearArrays();
    
    // initialization functions
    void reInit(int data_size, int seed);
    unsigned int generateUID(const float, const float) const;
    void prepareOcean();
    void setupGrid();

    // tessendorf ocean functions
    void evaluateHokData();
    void evaluateHieghtField();
    void evaluateChopField();
    void evaluateJacobians();
    void evaluateNormal();
    
    // interpolation functions
    inline float catmullRom(const float t, const float a, const float b, const float c, const float d) const;
    inline int wrap(int x) const;

    void getArrayType(aaOcean::arrayType type, float*& outArray) const;
};

#endif  /* AAOCEANCLASS_H */

