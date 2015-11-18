/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2015  <copyright holder> <email>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "gpuinfo.h"

GpuInfo::GpuInfo(QGLContext*& glContext)
{
  //Initializing
  m_curGlContext = NULL;
  
  m_curGpu.availMem = 0;
  m_curGpu.totalMem = 0;
  m_curGpu.gpuMake = NONE;
  
  setGlContext(glContext);
  setGpuMake();
}

GpuInfo::~GpuInfo()
{

}

void GpuInfo::setGlContext(QGLContext*& glContext)
{
  if(glContext->isValid())
    m_curGlContext = glContext->contextHandle();
}

void GpuInfo::setGpuMake()
{
  if(m_curGlContext->isValid())
  {
    char* rawMake = (char*) m_curGlContext->functions()->glGetString(GL_VENDOR);
    if(strcmp(rawMake, "NVIDIA Corporation") == 0)
      m_curGpu.gpuMake = NVIDIA;
    else if (strcmp(rawMake, "ATI Technologies") == 0)
      m_curGpu.gpuMake = ATI;
    else if(strcmp(rawMake, "Intel Open Source Technology Center") == 0)
      m_curGpu.gpuMake = INTEL;
    else
      m_curGpu.gpuMake = NONE; //Unknown vendor string or error
  }
}

GpuInfo::gpuvendors GpuInfo::getGpuMake()
{
  return m_curGpu.gpuMake;
}

GLint GpuInfo::getAvailMem()
{
  if(m_curGlContext->isValid())
  {
    GLint gpuMetrics [] = {0, 0, 0, 0};
    switch (getGpuMake())
    {
      case NVIDIA:
        if(m_curGlContext->hasExtension("GL_NVX_gpu_memory_info"))
        {
            #define GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX 0x9049
            m_curGlContext->functions()->glGetIntegerv(GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX,
                                               gpuMetrics);
        }
        break;
      case ATI:
        if(m_curGlContext->hasExtension("GL_ATI_meminfo"))
        {
            #define VBO_FREE_MEMORY_ATI 0x87FB
            m_curGlContext->functions()->glGetIntegerv(VBO_FREE_MEMORY_ATI,
                                                       gpuMetrics);
        }
        break;
      case INTEL:
        //Not sure how to get Intel stats yet
        return 0;
        break;
      default: //Unsupported card
        return 0; 
        break;
    }
    return gpuMetrics[0];
  }
  return 0; //Context needs to be valid
}

GLint GpuInfo::getTotalMem()
{
  if(m_curGlContext->isValid())
  {    
    switch (getGpuMake())
    {
      case NVIDIA:
      {
        if(m_curGlContext->hasExtension("GL_NVX_gpu_memory_info"))
        {
          #define GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX 0x9048
          GLint nvTotalMem [] = {0, 0, 0, 0};

          m_curGlContext->functions()->glGetIntegerv(GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX,
                                                     nvTotalMem);
          return nvTotalMem[0];
        }
        break;
      }
      case ATI:
	// This code doesn't work yet.  I need to find the library/extension that these AMD OpenGL
	// methods are defined in.
/*
	UINT n = wglGetGPUIDsAMD(0, 0);
	UINT *ids = new UINT[n];
	size_t total_mem_mb = 0;
	wglGetGPUIDsAMD(n, ids);
	wglGetGPUInfoAMD(ids[0],
		  WGL_GPU_RAM_AMD, 
		  GL_UNSIGNED_INT, 
		  sizeof(size_t),
	          &total_mem_mb);
*/
        break;
      case INTEL:
	//Not sure how to get Intel stats yet
	break;
      default: //Unsupported card
	break;
    } 
    return 0; //Cannot get memory info
  }
  return 0; //Context needs to be valid
}
