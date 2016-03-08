#include "aaOcean_init.h"

namespace aaOceanBSDCommand		{	extern void	initialize ();	};
namespace aaOceanBSDDeformer		{	extern void	initialize ();	};
namespace aaOceanBSDTextureNamespace		{	extern void	initialize ();	};
namespace aaOceanBSDChanModNameSpace		{	extern void	initialize ();	};

void initialize ()
{
    aaOceanBSDCommand :: initialize ();
    aaOceanBSDDeformer :: initialize ();
    aaOceanBSDTextureNamespace :: initialize ();
    aaOceanBSDChanModNameSpace :: initialize();
}
