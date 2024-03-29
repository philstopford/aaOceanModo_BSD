#include "aaOcean_texture.h"
#include <lxu_math.hpp>
#include <fstream>
#include <assert.h>
#include <lxvmath.h>
#include <lx_vector.hpp>
#include <lxu_matrix.hpp>
#include <lxvalue.h>

using namespace aaOceanBSDTextureNamespace;

LXtTagInfoDesc	 aaOceanBSDTexture::descInfo[] = {
    { LXsSRV_USERNAME,	"aaOceanBSD Texture" },
    { LXsSRV_LOGSUBSYSTEM,	"val-texture"	},
    { 0 }
};

#define SRVs_TEXTURE		"aaOceanBSD.texture"
#define SRVs_ITEMTYPE		SRVs_TEXTURE
// OS X defines this. MSVC doesn't.
#ifndef M_PI
#define M_PI        3.14159265358979323846264338327950288   /* pi             */
#endif

aaOceanBSDTexture::aaOceanBSDTexture ()
{
    my_type = LXiTYPE_NONE;
    dispAmplitude = 0.0;
}

aaOceanBSDTexture::~aaOceanBSDTexture ()
{
}

LXtItemType aaOceanBSDTexture::MyType ()
{
    if (my_type != LXiTYPE_NONE)
        return my_type;
    
    CLxUser_SceneService	 svc;
    
    my_type = svc.ItemType (SRVs_ITEMTYPE);
    return my_type;
}

static LXtTextValueHint hint_resolution[] = {
    4,			"Map size : 16",
    5,			"Map size : 32",
    6,			"Map size : 64",
    7,			"Map size : 128",
    8,			"Map size : 256",
    9,			"Map size : 512",
    10,			"Map size : 1024",
    11,			"Map size : 2048",
    12,			"Map size : 4096",
    13,			"Map size : 8192 (memory hungry!)",
    14,			"Map size : 16384 (very memory hungry!)",
    4,			NULL
};

static LXtTextValueHint hint_outputType[] = {
    0,			"Heightfield and chop",
    1,          "Foam",
    2,          "Eigenminus",
    3,			"Eigenplus",
    4,          "Tangent h and c",
    5,          "Tangent h and c ff",
    6,          "Testing",
    0,			NULL
};

static LXtTextValueHint hint_boolLimit[] = {
    0,			"&min",		// int min 0
    1,			"&max",		// int max 1
    -1,			NULL
};

static LXtTextValueHint hint_spectrum[] = {
    0,			"Philips",
    1,			"Pierson-Morkowitz",
    2,			"TMA",
    0,          NULL
};

/*
 * Setup channels for the item type.
 */
LxResult aaOceanBSDTexture::vtx_SetupChannels (ILxUnknownID addChan)
{
    CLxUser_AddChannel	 ac (addChan);
    
    ac.NewChannel("tone", LXsTYPE_BOOLEAN);
    ac.SetDefault(0.0, 1);
    
    ac.NewChannel("divide", LXsTYPE_FLOAT);
    ac.SetDefault(1.0f, 0);

    ac.NewChannel  ("outputType",	LXsTYPE_INTEGER);
    ac.SetDefault  (0.0, 0);
    ac.SetHint(hint_outputType);
    
    ac.NewChannel  ("resolution",	LXsTYPE_INTEGER);
    ac.SetDefault  (0.0, 4);
    ac.SetHint(hint_resolution);
    
    ac.NewChannel  ("oceanSize",			LXsTYPE_FLOAT);
    ac.SetDefault  (100.0f, 0);
    
    ac.NewChannel  ("waveHeight",	LXsTYPE_FLOAT);
    ac.SetDefault  (2.0f, 0);
    
    ac.NewChannel  ("surfaceTension",	LXsTYPE_FLOAT);
    ac.SetDefault  (0.0f, 0);
    
    ac.NewChannel  ("waveAlign",	LXsTYPE_INTEGER);
    ac.SetDefault  (0.0, 1);
    
    ac.NewChannel  ("waveSmooth",	LXsTYPE_FLOAT);
    ac.SetDefault  (0.0f, 0);
    
    ac.NewChannel  ("waveDirection",	LXsTYPE_FLOAT);
    ac.SetDefault  (45.0f, 0);
    
    ac.NewChannel  ("waveReflection",	LXsTYPE_FLOAT);
    ac.SetDefault  (0.0f, 0);
    
    ac.NewChannel  ("waveSpeed",	LXsTYPE_FLOAT);
    ac.SetDefault  (1.0f, 0);
    
    ac.NewChannel  ("waveChop",	LXsTYPE_FLOAT);
    ac.SetDefault  (2.0f, 0);
    
    ac.NewChannel  ("oceanDepth",	LXsTYPE_FLOAT);
    ac.SetDefault  (10000.0f, 0);
    
    ac.NewChannel  ("waveSize",	LXsTYPE_FLOAT);
    ac.SetDefault  (4.0f, 0);
    
    ac.NewChannel  ("seed",	LXsTYPE_INTEGER);
    ac.SetDefault  (0.0f, 1);
    
    ac.NewChannel  ("spectrum",	LXsTYPE_INTEGER);
    ac.SetDefault  (0.0f, 0);
    ac.SetHint(hint_spectrum);
    
    ac.NewChannel  ("repeatTime",	LXsTYPE_FLOAT);
    ac.SetDefault  (1000.0f, 0);
    
    ac.NewChannel  ("doFoam",	LXsTYPE_INTEGER);
    ac.SetDefault  (0.0, 0);
    ac.SetHint(hint_boolLimit);
    
    ac.NewChannel  ("foamRange",	LXsTYPE_FLOAT);
    ac.SetDefault  (1.0f, 0);
    
    ac.NewChannel  ("foamMax",	LXsTYPE_FLOAT);
    ac.SetDefault  (1.0f, 0);
    
    ac.NewChannel  ("randWeight",	LXsTYPE_FLOAT);
    ac.SetDefault  (0.0f, 0);
    
    ac.NewChannel  ("specMult",	LXsTYPE_FLOAT);
    ac.SetDefault  (1.0f, 0);
    
    ac.NewChannel  ("peakSharpening",	LXsTYPE_FLOAT);
    ac.SetDefault  (1.0f, 0);
    
    ac.NewChannel  ("swell",	LXsTYPE_FLOAT);
    ac.SetDefault  (0.0f, 0);
    
    ac.NewChannel  ("tmaFetch",	LXsTYPE_FLOAT);
    ac.SetDefault  (20.0f, 0);
    
    ac.NewChannel("newFoam", LXsTYPE_BOOLEAN);
    ac.SetDefault(0.0, 1);
    
    ac.NewChannel  ("timeOffset",	LXsTYPE_FLOAT);
    ac.SetDefault  (0.0f, 0);

    return LXe_OK;
}

/*
 * Attach to channel evaluations. This gets the indicies for the channels in
 * in attributes.
 */
LxResult aaOceanBSDTexture::vtx_LinkChannels (ILxUnknownID eval, ILxUnknownID	item)
{
    CLxUser_Evaluation	 ev (eval);
    
    m_idx_tone = ev.AddChan (item, "tone");
    m_idx_divide = ev.AddChan (item, "divide");
    m_idx_outputType = ev.AddChan (item, "outputType");
    m_idx_resolution = ev.AddChan (item, "resolution");
    m_idx_oceanSize = ev.AddChan (item, "oceanSize");
    m_idx_waveHeight = ev.AddChan (item, "waveHeight");
    m_idx_waveSize = ev.AddChan (item, "waveSize");
    m_idx_surfaceTension = ev.AddChan (item, "surfaceTension");
    m_idx_waveAlign = ev.AddChan (item, "waveAlign");
    m_idx_waveSmooth = ev.AddChan (item, "waveSmooth");
    m_idx_waveDirection = ev.AddChan (item, "waveDirection");
    m_idx_waveReflection = ev.AddChan (item, "waveReflection");
    m_idx_waveSpeed = ev.AddChan (item, "waveSpeed");
    m_idx_waveChop = ev.AddChan (item, "waveChop");
    m_idx_oceanDepth = ev.AddChan (item, "oceanDepth");
    m_idx_seed = ev.AddChan(item, "seed");
    m_idx_spectrum = ev.AddChan(item, "spectrum");
    m_idx_repeatTime = ev.AddChan(item, "repeatTime");
    m_idx_doFoam = ev.AddChan(item, "doFoam");
    m_idx_foamRange = ev.AddChan(item, "foamRange");
    m_idx_foamMax = ev.AddChan(item, "foamMax");
    m_idx_randWeight = ev.AddChan(item, "randWeight");
    m_idx_peakSharpening = ev.AddChan(item, "peakSharpening");
    m_idx_specMult = ev.AddChan(item, "specMult");
    m_idx_jswpfetch = ev.AddChan(item, "tmaFetch");
    m_idx_swell = ev.AddChan(item, "swell");
    m_idx_newFoam = ev.AddChan(item, "newFoam");
    m_idx_timeOffset = ev.AddChan(item, "timeOffset");
    
    // m_idx_time = ev.AddChan (item, "time");
    
    m_idx_time = ev.AddTime ();
    
    tin_offset = pkt_service.GetOffset (LXsCATEGORY_SAMPLE, LXsP_TEXTURE_INPUT);
    tinDsp_offset = pkt_service.GetOffset (LXsCATEGORY_SAMPLE, LXsP_DISPLACE);
    nrm_offset  = pkt_service.GetOffset (LXsCATEGORY_SAMPLE, LXsP_SURF_NORMAL);
    pos_offset  = pkt_service.GetOffset (LXsCATEGORY_SAMPLE, LXsP_SAMPLE_POSITION);
    
    return LXe_OK;
}

/*
 * Read channel values which may have changed. These are stored in the allocated
 * data for later evaluation.
 */
LxResult aaOceanBSDTexture::vtx_ReadChannels(ILxUnknownID attr, void  **ppvData)
{
    CLxUser_Attributes	 at (attr);
    
    std::unique_ptr<OceanData> newOceanData(new OceanData());
    
    tone = at.Bool(m_idx_tone);
    divide = at.Int(m_idx_divide);
    if (divide < 1E-9)
    {
        divide = 1E-9;
    }
    
    newOceanData->m_outputType = at.Int(m_idx_outputType);
    if(newOceanData->m_outputType > 6)
    {
        newOceanData->m_outputType = 6;
    }
    if(newOceanData->m_outputType < 0)
    {
        newOceanData->m_outputType = 0;
    }
    
    newOceanData->m_resolution = at.Int(m_idx_resolution);
    if(newOceanData->m_resolution > 14)
    {
        newOceanData->m_resolution = 14;
    }
    if(newOceanData->m_resolution < 4)
    {
        newOceanData->m_resolution = 4;
    }
    
    newOceanData->m_oceanSize = at.Float(m_idx_oceanSize);
    if(newOceanData->m_oceanSize <= 0)
    {
        newOceanData->m_oceanSize = 0.01;
    }
    newOceanData->m_waveHeight = at.Float(m_idx_waveHeight);
    newOceanData->m_waveSize = at.Float(m_idx_waveSize);
    newOceanData->m_surfaceTension = at.Float(m_idx_surfaceTension);
    newOceanData->m_waveAlign = at.Int(m_idx_waveAlign);
    newOceanData->m_waveSmooth = at.Float(m_idx_waveSmooth);
    newOceanData->m_waveDirection = at.Float(m_idx_waveDirection);
    newOceanData->m_waveReflection = at.Float(m_idx_waveReflection);
    newOceanData->m_waveSpeed = at.Float(m_idx_waveSpeed);
    if(newOceanData->m_waveSpeed <= 0)
    {
        newOceanData->m_waveSpeed = 0.01;
    }
    newOceanData->m_waveChop = at.Float(m_idx_waveChop);
    newOceanData->m_oceanDepth = at.Float(m_idx_oceanDepth);
    newOceanData->m_seed = at.Int(m_idx_seed);
    newOceanData->m_spectrum = at.Int(m_idx_spectrum);
    if (newOceanData->m_spectrum < 0)
    {
        newOceanData->m_spectrum = 0;
    }
    if (newOceanData->m_spectrum > 2)
    {
        newOceanData->m_spectrum = 2;
    }
    newOceanData->m_repeatTime = at.Float(m_idx_repeatTime);
    newOceanData->m_doFoam = (bool) at.Int(m_idx_doFoam);
    newOceanData->foamRange = at.Float(m_idx_foamRange);
    newOceanData->foamMax = at.Float(m_idx_foamMax);
    newOceanData->m_doNormals = false; // disabled due to Vector issues (bool) at.Int(m_idx_doNormals);
    newOceanData->m_randWeight = at.Float(m_idx_randWeight);
    if(newOceanData->m_randWeight > 1)
    {
        newOceanData->m_randWeight = 1.0f;
    }
    if(newOceanData->m_randWeight < 0)
    {
        newOceanData->m_randWeight = 0.0f;
    }
    
    newOceanData->m_specMult = at.Float(m_idx_specMult);
    if(newOceanData->m_specMult > 100)
    {
        newOceanData->m_specMult = 100.0f;
    }
    if(newOceanData->m_specMult < 1)
    {
        newOceanData->m_specMult = 1.0f;
    }
    
    newOceanData->m_peakSharpening = at.Float(m_idx_peakSharpening);
    if(newOceanData->m_peakSharpening > 6)
    {
        newOceanData->m_peakSharpening = 6.0f;
    }
    if(newOceanData->m_peakSharpening < 0.001f)
    {
        newOceanData->m_peakSharpening = 0.001f;
    }
    
    newOceanData->m_jswpfetch = at.Float(m_idx_jswpfetch);
    if(newOceanData->m_jswpfetch > 1000)
    {
        newOceanData->m_jswpfetch = 1000;
    }
    if(newOceanData->m_jswpfetch < 0.001f)
    {
        newOceanData->m_jswpfetch = 0.001f;
    }
    
    newOceanData->m_swell = at.Float(m_idx_swell);
    if(newOceanData->m_swell > 1)
    {
        newOceanData->m_swell = 1;
    }
    if(newOceanData->m_swell < 0)
    {
        newOceanData->m_swell = 0;
    }
    
    newOceanData->newFoam = at.Bool(m_idx_newFoam);
 
    newOceanData->m_timeOffset = at.Float(m_idx_timeOffset);
    
    newOceanData->m_time = at.Float(m_idx_time);
        
    maybeResetOceanData(std::move(newOceanData));
    
    return LXe_OK;
}

void aaOceanBSDTexture::maybeResetOceanData(std::unique_ptr<OceanData> newOceanData) {
    if (oceanData_.get() == nullptr || *newOceanData != *oceanData_) {
        // Automatically unlocks the mutex when it goes out of scope.
        std::lock_guard<std::mutex> lock(myMutex_);
        // Check it again inside the mutex because another thread may have overridden it.
        if (oceanData_.get() == nullptr || *newOceanData != *oceanData_) {
            oceanData_ = std::move(newOceanData); // newOceanData is no longer valid, do not use it again in this function!
            mOcean_.input(  oceanData_->m_resolution,
                          oceanData_->m_spectrum,
                          (unsigned int)oceanData_->m_seed,
                          oceanData_->m_oceanSize,
                          oceanData_->m_oceanDepth,
                          oceanData_->m_surfaceTension,
                          oceanData_->m_waveSize,
                          oceanData_->m_waveSmooth,
                          oceanData_->m_waveDirection,
                          oceanData_->m_waveAlign,
                          oceanData_->m_waveReflection,
                          oceanData_->m_waveSpeed,
                          oceanData_->m_waveHeight,
                          oceanData_->m_waveChop,
                          oceanData_->m_time + oceanData_->m_timeOffset,
                          oceanData_->m_repeatTime,
                          oceanData_->m_doFoam,
                          oceanData_->m_randWeight,
                          oceanData_->m_specMult,
                          oceanData_->m_peakSharpening,
                          oceanData_->m_jswpfetch,
                          oceanData_->m_swell);
            mOcean_.m_foamBoundrange = oceanData_->foamRange;
            mOcean_.m_foamBoundmax = oceanData_->foamMax;
            mOcean_.m_newFoam = oceanData_->newFoam;
            // clear arrays that are not required during shader evaluation
			// Disabled as this crashes the library - yay.
            // mOcean_.clearResidualArrays();
        }
    }
}


void aaOceanBSDTexture::vtx_Evaluate (ILxUnknownID etor, int *idx, ILxUnknownID vector, LXpTextureOutput *tOut, void *data)
{
    LXpTextureInput		*tInp;
    LXpDisplace *tInpDsp;
    LXpSampleSurfNormal	 *sNrm;
    LXpSamplePosition	 *sPosition;
    
    tInp = (LXpTextureInput *) pkt_service.FastPacket (vector, tin_offset);
    tInpDsp = (LXpDisplace *) pkt_service.FastPacket (vector, tinDsp_offset);
    sNrm	  = (LXpSampleSurfNormal*) pkt_service.FastPacket(vector, nrm_offset);
    sPosition = (st_LXpSamplePosition*) pkt_service.FastPacket(vector, pos_offset);
    
    if (tInpDsp->amplitude != 0)
    {
        dispAmplitude = tInpDsp->amplitude;
    }
    
    if (dispAmplitude <= 0.0)
        return;
    
    // Ensure that a valid normal is given
    if ( !LXx_VNEZERO(sNrm->gNorm) )
        return;
    
    if (!LXx_VNEZERO(tInp->dpdu) || !LXx_VNEZERO(tInp->dpdv) )
        return;
    
    CLxVector dpdu = CLxVector (tInp->dpdu).normal();
    CLxVector dpdv = CLxVector (tInp->dpdv).normal();
	CLxVector norm = dpdu.cross(dpdv).normal(); // (dpdu ^ dpdv).normal();
    
    CLxMatrix4 tangentMatrix = CLxMatrix4();
    
    LXx_VCPY(tangentMatrix[0], dpdu.v);
    LXx_VCPY(tangentMatrix[1], dpdv.v);
    LXx_VCPY(tangentMatrix[2], norm.v);
    
    float result[3]; // vector for the color output.
    result[0] = result[1] = result[2] = 0.0;
    float value = 0.0; // value output
    float alpha = 1.0; // alpha output
    

    float u_oPos = sPosition->wPos[0] / mOcean_.m_oceanScale;
    float v_oPos = -sPosition->wPos[2] / mOcean_.m_oceanScale; // aaOcean has a hardcoded sign inversion for the 'V'

    tOut->direct   = 1;
    // The intent of tInpDsp->enable isn't entirely clear. The docs, such as they are, indicate that the texture should set this when outputting displacement.
    tInpDsp->enable = true;
    
    result[1] = mOcean_.getOceanData(u_oPos, v_oPos, aaOcean::eHEIGHTFIELD);/* +w_oPos;*/

    if (mOcean_.isChoppy())
    {
        result[0] = mOcean_.getOceanData(u_oPos, v_oPos, aaOcean::eCHOPX);
        result[2] = mOcean_.getOceanData(u_oPos, v_oPos, aaOcean::eCHOPZ);
    } else {
        result[0] = 0.0;
        result[2] = 0.0;
    }
        
    // This is the new position we want to apply in world space
    CLxVector destPosition(result[0], result[1], result[2]);
    
    CLxMatrix4 positionMatrix = CLxMatrix4();
    
    CLxVector origin;
    LXx_VCPY(origin, sPosition->uPos);
    
    LXx_VSCL(origin, mOcean_.m_oceanScale);
    
    // This is to avoid clipping. Not sure if this is the correct parameter to use for the purpose.
    // I eyeballed the scaling value of 141, it seems to match the deformer. Not sure where the difference comes from
    // This effectively ignores the "Displacement Distance" value and removes the clipping if big enough.
    
    // object position used here as the ocean wraps to the ocean tile size, so the remainder is the object coordinate.
    
    CLxVector localPosition = destPosition - origin;
    
    //localPosition.v[1] /= (dispAmplitude * 150); // compensate for displacement on material
    
    LXx_VSCL(localPosition, 1/(dispAmplitude * divide));
    
    positionMatrix.setTranslation( localPosition );
    
    CLxMatrix4 matResult = positionMatrix * tangentMatrix.inverse();
    
    CLxVector outVector = matResult.getTranslation();
    
    if(oceanData_->m_outputType == 0) // normal displacement texture configuration
    {
        // Note that modo expects textures to output the right kind of data based on the context. This is the reason for checking against
        // LXi_TFX_COLOR in the context below. If we aren't driving a color, we output a value instead.
        if (LXi_TFX_COLOR == tInp->context)
        {
            if (tone)
            {
                LXx_VSCL(outVector, -1.0f);
            }
            
            LXx_VCPY (tOut->color[0], outVector);

        }
        
        value = result[1];// * rd->m_waveHeight; // in case displacement is used rather than vector displacement.
    }
    
    if(oceanData_->m_doFoam == true)
    {
        // Foam is always output in world space, so can be mapped straight to surface UVs without taking vector displacement into account. (per Amaan)
        
        float foamResult = 0.0f;
        
        foamResult = mOcean_.getOceanData(u_oPos, v_oPos, aaOcean::eFOAM);
		foamResult = 1.0 - foamResult;

        if (tone)
        {
            foamResult = 1.0f - foamResult;
        }
        if(oceanData_->m_outputType == 1) // foam map requested
        {
            value = foamResult;
        }
        if(oceanData_->m_outputType == 0) // normal displacement texture configuration
        {
            value = foamResult;
        }
        
    }
    
    if(oceanData_->m_outputType == 2) // Eigenvalues - minus
    {
        if(oceanData_->m_doFoam == true)
        {
            result[0] = mOcean_.getOceanData(u_oPos, v_oPos, aaOcean::eEIGENMINUSX);
            result[2] = mOcean_.getOceanData(u_oPos, v_oPos, aaOcean::eEIGENMINUSZ);
        }
        LXx_VCPY (tOut->color[0], result);
    }
    if(oceanData_->m_outputType == 3) // Eigenvalues - plus
    {
        if(oceanData_->m_doFoam == true)
        {
            result[0] = mOcean_.getOceanData(u_oPos, v_oPos, aaOcean::eEIGENPLUSX);
            result[2] = mOcean_.getOceanData(u_oPos, v_oPos, aaOcean::eEIGENPLUSZ);
        }
        LXx_VCPY (tOut->color[0], result);
    }
    
    tOut->value[0] = value;
    tOut->alpha[0] = alpha;
    bool debugMe = false;
    if (debugMe)
    {
        std::ofstream fout ("/Users/phil/aadebug_texture.csv", std::ios::app);
        std::string tmpString =
        std::to_string(u_oPos) + "," + std::to_string(v_oPos) + "," +
        //std::to_string(tOut->color[0][0]) + "," + std::to_string(tOut->color[0][1]) + "," + std::to_string(tOut->color[0][2]) + "\n";
        std::to_string(tOut->value[0])+ "\n";
        fout << tmpString;
        fout.close();
    }

}

LxResult aaOceanBSDTexture::vtx_Customize(ILxUnknownID customId, void **ppvData)
{
    CLxLoc_ValueTextureCustom	cust(customId);
    cust.AddFeature(LXiTBLX_BASEFEATURE, LXsTBLX_FEATURE_NORMAL );
    cust.AddFeature(LXiTBLX_BASEFEATURE, LXsTBLX_FEATURE_POS );
    
    return LXe_OK;
}

/*
 * Release the cached state after rendering is complete.
 */
void aaOceanBSDTexture::vtx_Cleanup (void	*data)
{
}
