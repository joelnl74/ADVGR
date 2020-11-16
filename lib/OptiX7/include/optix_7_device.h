/*
* Copyright (c) 2020 NVIDIA Corporation.  All rights reserved.
*
* NVIDIA Corporation and its licensors retain all intellectual property and proprietary
* rights in and to this software, related documentation and any modifications thereto.
* Any use, reproduction, disclosure or distribution of this software and related
* documentation without an express license agreement from NVIDIA Corporation is strictly
* prohibited.
*
* TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE LAW, THIS SOFTWARE IS PROVIDED *AS IS*
* AND NVIDIA AND ITS SUPPLIERS DISCLAIM ALL WARRANTIES, EITHER EXPRESS OR IMPLIED,
* INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
* PARTICULAR PURPOSE.  IN NO EVENT SHALL NVIDIA OR ITS SUPPLIERS BE LIABLE FOR ANY
* SPECIAL, INCIDENTAL, INDIRECT, OR CONSEQUENTIAL DAMAGES WHATSOEVER (INCLUDING, WITHOUT
* LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS, BUSINESS INTERRUPTION, LOSS OF
* BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS) ARISING OUT OF THE USE OF OR
* INABILITY TO USE THIS SOFTWARE, EVEN IF NVIDIA HAS BEEN ADVISED OF THE POSSIBILITY OF
* SUCH DAMAGES
*/

/// @file
/// @author NVIDIA Corporation
/// @brief  OptiX public API header
///
/// OptiX public API Reference - Device API declarations

#if !defined( __OPTIX_INCLUDE_INTERNAL_HEADERS__ )
#error("optix_7_device.h is an internal header file and must not be used directly.  Please use optix_device.h or optix.h instead.")
#endif


#ifndef __optix_optix_7_device_h__
#define __optix_optix_7_device_h__

#if defined( __cplusplus ) && ( __cplusplus < 201103L ) && !defined( _WIN32 )
#error Device code for OptiX requires at least C++11. Consider adding "--std c++11" to the nvcc command-line.
#endif

#include "optix_7_types.h"

/// \defgroup optix_device_api Device API
/// \brief OptiX Device API

/** \addtogroup optix_device_api
@{
*/

/// Initiates a ray tracing query starting with the given traversable (overload without payload).
///
/// \param[in] handle
/// \param[in] rayOrigin
/// \param[in] rayDirection
/// \param[in] tmin
/// \param[in] tmax
/// \param[in] rayTime
/// \param[in] visibilityMask really only 8 bits
/// \param[in] rayFlags       really only 8 bits, combination of OptixRayFlags
/// \param[in] SBToffset      really only 8 bits
/// \param[in] SBTstride      really only 8 bits
/// \param[in] missSBTIndex   specifies the miss program invoked on a miss
static __forceinline__ __device__ void optixTrace( OptixTraversableHandle handle,
                                                   float3                 rayOrigin,
                                                   float3                 rayDirection,
                                                   float                  tmin,
                                                   float                  tmax,
                                                   float                  rayTime,
                                                   OptixVisibilityMask    visibilityMask,
                                                   unsigned int           rayFlags,
                                                   unsigned int           SBToffset,
                                                   unsigned int           SBTstride,
                                                   unsigned int           missSBTIndex );

/// Initiates a ray tracing query starting with the given traversable (overload with 1 payload register).
///
/// \see #optixTrace(OptixTraversableHandle,float3,float3,float,float,float,OptixVisibilityMask,unsigned int,unsigned int,unsigned int,unsigned int)
static __forceinline__ __device__ void optixTrace( OptixTraversableHandle handle,
                                                   float3                 rayOrigin,
                                                   float3                 rayDirection,
                                                   float                  tmin,
                                                   float                  tmax,
                                                   float                  rayTime,
                                                   OptixVisibilityMask    visibilityMask,
                                                   unsigned int           rayFlags,
                                                   unsigned int           SBToffset,
                                                   unsigned int           SBTstride,
                                                   unsigned int           missSBTIndex,
                                                   unsigned int&          p0 );

/// Initiates a ray tracing query starting with the given traversable (overload with 2 payload registers).
///
/// \see #optixTrace(OptixTraversableHandle,float3,float3,float,float,float,OptixVisibilityMask,unsigned int,unsigned int,unsigned int,unsigned int)
static __forceinline__ __device__ void optixTrace( OptixTraversableHandle handle,
                                                   float3                 rayOrigin,
                                                   float3                 rayDirection,
                                                   float                  tmin,
                                                   float                  tmax,
                                                   float                  rayTime,
                                                   OptixVisibilityMask    visibilityMask,
                                                   unsigned int           rayFlags,
                                                   unsigned int           SBToffset,
                                                   unsigned int           SBTstride,
                                                   unsigned int           missSBTIndex,
                                                   unsigned int&          p0,
                                                   unsigned int&          p1 );

/// Initiates a ray tracing query starting with the given traversable (overload with 3 payload registers).
///
/// \see #optixTrace(OptixTraversableHandle,float3,float3,float,float,float,OptixVisibilityMask,unsigned int,unsigned int,unsigned int,unsigned int)
static __forceinline__ __device__ void optixTrace( OptixTraversableHandle handle,
                                                   float3                 rayOrigin,
                                                   float3                 rayDirection,
                                                   float                  tmin,
                                                   float                  tmax,
                                                   float                  rayTime,
                                                   OptixVisibilityMask    visibilityMask,
                                                   unsigned int           rayFlags,
                                                   unsigned int           SBToffset,
                                                   unsigned int           SBTstride,
                                                   unsigned int           missSBTIndex,
                                                   unsigned int&          p0,
                                                   unsigned int&          p1,
                                                   unsigned int&          p2 );

/// Initiates a ray tracing query starting with the given traversable (overload with 4 payload registers).
///
/// \see #optixTrace(OptixTraversableHandle,float3,float3,float,float,float,OptixVisibilityMask,unsigned int,unsigned int,unsigned int,unsigned int)
static __forceinline__ __device__ void optixTrace( OptixTraversableHandle handle,
                                                   float3                 rayOrigin,
                                                   float3                 rayDirection,
                                                   float                  tmin,
                                                   float                  tmax,
                                                   float                  rayTime,
                                                   OptixVisibilityMask    visibilityMask,
                                                   unsigned int           rayFlags,
                                                   unsigned int           SBToffset,
                                                   unsigned int           SBTstride,
                                                   unsigned int           missSBTIndex,
                                                   unsigned int&          p0,
                                                   unsigned int&          p1,
                                                   unsigned int&          p2,
                                                   unsigned int&          p3 );

/// Initiates a ray tracing query starting with the given traversable (overload with 5 payload registers).
///
/// \see #optixTrace(OptixTraversableHandle,float3,float3,float,float,float,OptixVisibilityMask,unsigned int,unsigned int,unsigned int,unsigned int)
static __forceinline__ __device__ void optixTrace( OptixTraversableHandle handle,
                                                   float3                 rayOrigin,
                                                   float3                 rayDirection,
                                                   float                  tmin,
                                                   float                  tmax,
                                                   float                  rayTime,
                                                   OptixVisibilityMask    visibilityMask,
                                                   unsigned int           rayFlags,
                                                   unsigned int           SBToffset,
                                                   unsigned int           SBTstride,
                                                   unsigned int           missSBTIndex,
                                                   unsigned int&          p0,
                                                   unsigned int&          p1,
                                                   unsigned int&          p2,
                                                   unsigned int&          p3,
                                                   unsigned int&          p4 );

/// Initiates a ray tracing query starting with the given traversable (overload with 6 payload registers).
///
/// \see #optixTrace(OptixTraversableHandle,float3,float3,float,float,float,OptixVisibilityMask,unsigned int,unsigned int,unsigned int,unsigned int)
static __forceinline__ __device__ void optixTrace( OptixTraversableHandle handle,
                                                   float3                 rayOrigin,
                                                   float3                 rayDirection,
                                                   float                  tmin,
                                                   float                  tmax,
                                                   float                  rayTime,
                                                   OptixVisibilityMask    visibilityMask,
                                                   unsigned int           rayFlags,
                                                   unsigned int           SBToffset,
                                                   unsigned int           SBTstride,
                                                   unsigned int           missSBTIndex,
                                                   unsigned int&          p0,
                                                   unsigned int&          p1,
                                                   unsigned int&          p2,
                                                   unsigned int&          p3,
                                                   unsigned int&          p4,
                                                   unsigned int&          p5 );

/// Initiates a ray tracing query starting with the given traversable (overload with 7 payload registers).
///
/// \see #optixTrace(OptixTraversableHandle,float3,float3,float,float,float,OptixVisibilityMask,unsigned int,unsigned int,unsigned int,unsigned int)
static __forceinline__ __device__ void optixTrace( OptixTraversableHandle handle,
                                                   float3                 rayOrigin,
                                                   float3                 rayDirection,
                                                   float                  tmin,
                                                   float                  tmax,
                                                   float                  rayTime,
                                                   OptixVisibilityMask    visibilityMask,
                                                   unsigned int           rayFlags,
                                                   unsigned int           SBToffset,
                                                   unsigned int           SBTstride,
                                                   unsigned int           missSBTIndex,
                                                   unsigned int&          p0,
                                                   unsigned int&          p1,
                                                   unsigned int&          p2,
                                                   unsigned int&          p3,
                                                   unsigned int&          p4,
                                                   unsigned int&          p5,
                                                   unsigned int&          p6 );

/// Initiates a ray tracing query starting with the given traversable (overload with 8 payload registers).
///
/// \see #optixTrace(OptixTraversableHandle,float3,float3,float,float,float,OptixVisibilityMask,unsigned int,unsigned int,unsigned int,unsigned int)
static __forceinline__ __device__ void optixTrace( OptixTraversableHandle handle,
                                                   float3                 rayOrigin,
                                                   float3                 rayDirection,
                                                   float                  tmin,
                                                   float                  tmax,
                                                   float                  rayTime,
                                                   OptixVisibilityMask    visibilityMask,
                                                   unsigned int           rayFlags,
                                                   unsigned int           SBToffset,
                                                   unsigned int           SBTstride,
                                                   unsigned int           missSBTIndex,
                                                   unsigned int&          p0,
                                                   unsigned int&          p1,
                                                   unsigned int&          p2,
                                                   unsigned int&          p3,
                                                   unsigned int&          p4,
                                                   unsigned int&          p5,
                                                   unsigned int&          p6,
                                                   unsigned int&          p7 );

/// Writes the 32-bit payload value at slot 0.
static __forceinline__ __device__ void optixSetPayload_0( unsigned int p );
/// Writes the 32-bit payload value at slot 1.
static __forceinline__ __device__ void optixSetPayload_1( unsigned int p );
/// Writes the 32-bit payload value at slot 2.
static __forceinline__ __device__ void optixSetPayload_2( unsigned int p );
/// Writes the 32-bit payload value at slot 3.
static __forceinline__ __device__ void optixSetPayload_3( unsigned int p );
/// Writes the 32-bit payload value at slot 4.
static __forceinline__ __device__ void optixSetPayload_4( unsigned int p );
/// Writes the 32-bit payload value at slot 5.
static __forceinline__ __device__ void optixSetPayload_5( unsigned int p );
/// Writes the 32-bit payload value at slot 6.
static __forceinline__ __device__ void optixSetPayload_6( unsigned int p );
/// Writes the 32-bit payload value at slot 7.
static __forceinline__ __device__ void optixSetPayload_7( unsigned int p );

/// Reads the 32-bit payload value at slot 0.
static __forceinline__ __device__ unsigned int optixGetPayload_0();
/// Reads the 32-bit payload value at slot 1.
static __forceinline__ __device__ unsigned int optixGetPayload_1();
/// Reads the 32-bit payload value at slot 2.
static __forceinline__ __device__ unsigned int optixGetPayload_2();
/// Reads the 32-bit payload value at slot 3.
static __forceinline__ __device__ unsigned int optixGetPayload_3();
/// Reads the 32-bit payload value at slot 4.
static __forceinline__ __device__ unsigned int optixGetPayload_4();
/// Reads the 32-bit payload value at slot 5.
static __forceinline__ __device__ unsigned int optixGetPayload_5();
/// Reads the 32-bit payload value at slot 6.
static __forceinline__ __device__ unsigned int optixGetPayload_6();
/// Reads the 32-bit payload value at slot 7.
static __forceinline__ __device__ unsigned int optixGetPayload_7();

/// Returns an undefined value.
static __forceinline__ __device__ unsigned int optixUndefinedValue();

/// Returns the rayOrigin passed into optixTrace.
///
/// May be more expensive to call in IS and AH than their object space counterparts,
/// so effort should be made to use the object space ray in those programs.
/// Only available in IS, AH, CH, MS
static __forceinline__ __device__ float3 optixGetWorldRayOrigin();

/// Returns the rayDirection passed into optixTrace.
///
/// May be more expensive to call in IS and AH than their object space counterparts,
/// so effort should be made to use the object space ray in those programs.
/// Only available in IS, AH, CH, MS
static __forceinline__ __device__ float3 optixGetWorldRayDirection();

/// Returns the current object space ray origin based on the current transform stack.
///
/// Only available in IS and AH.
static __forceinline__ __device__ float3 optixGetObjectRayOrigin();

/// Returns the current object space ray direction based on the current transform stack.
///
/// Only available in IS and AH.
static __forceinline__ __device__ float3 optixGetObjectRayDirection();

/// Returns the tmin passed into optixTrace.
///
/// Only available in IS, AH, CH, MS
static __forceinline__ __device__ float optixGetRayTmin();

/// In IS and CH returns the current smallest reported hitT or the tmax passed into optixTrace if no hit has been reported
/// In AH returns the hitT value as passed in to optixReportIntersection
/// In MS returns the tmax passed into optixTrace
/// Only available in IS, AH, CH, MS
static __forceinline__ __device__ float optixGetRayTmax();

/// Returns the rayTime passed into optixTrace.
///
/// Will return 0 if motion is disabled.
/// Only available in IS, AH, CH, MS
static __forceinline__ __device__ float optixGetRayTime();

/// Returns the rayFlags passed into optixTrace
///
/// Only available in IS, AH, CH, MS
static __forceinline__ __device__ unsigned int optixGetRayFlags();

/// Returns the visibilityMask passed into optixTrace
///
/// Only available in IS, AH, CH, MS
static __forceinline__ __device__ unsigned int optixGetRayVisibilityMask();

/// Return the object space triangle vertex positions of a given triangle in a Geometry
/// Acceleration Structure (GAS) at a given motion time.
///
/// If motion is disabled via OptixPipelineCompileOptions::usesMotionBlur, or the GAS does not contain motion, the
/// time parameter is ignored.
static __forceinline__ __device__ void optixGetTriangleVertexData( OptixTraversableHandle gas, unsigned int primIdx, unsigned int sbtGASIndex, float time, float3 data[3]);

/// Return the object space curve control vertex data of a linear curve in a Geometry
/// Acceleration Structure (GAS) at a given motion time.
///
/// data[i] = {x,y,z,w} with {x,y,z} the position and w the radius of control vertex i.
/// If motion is disabled via OptixPipelineCompileOptions::usesMotionBlur, or the GAS does not contain motion, the
/// time parameter is ignored.
static __forceinline__ __device__ void optixGetLinearCurveVertexData( OptixTraversableHandle gas, unsigned int primIdx, unsigned int sbtGASIndex, float time, float4 data[2] );

/// Return the object space curve control vertex data of a quadratic BSpline curve in a Geometry
/// Acceleration Structure (GAS) at a given motion time.
///
/// data[i] = {x,y,z,w} with {x,y,z} the position and w the radius of control vertex i.
/// If motion is disabled via OptixPipelineCompileOptions::usesMotionBlur, or the GAS does not contain motion, the
/// time parameter is ignored.
static __forceinline__ __device__ void optixGetQuadraticBSplineVertexData( OptixTraversableHandle gas, unsigned int primIdx, unsigned int sbtGASIndex, float time, float4 data[3] );

/// Return the object space curve control vertex data of a cubic BSpline curve in a Geometry
/// Acceleration Structure (GAS) at a given motion time.
///
/// data[i] = {x,y,z,w} with {x,y,z} the position and w the radius of control vertex i.
/// If motion is disabled via OptixPipelineCompileOptions::usesMotionBlur, or the GAS does not contain motion, the
/// time parameter is ignored.
static __forceinline__ __device__ void optixGetCubicBSplineVertexData( OptixTraversableHandle gas, unsigned int primIdx, unsigned int sbtGASIndex, float time, float4 data[4] );

/// Returns the traversable handle for the Geometry Acceleration Structure (GAS) containing
/// the current hit. May be called from IS, AH and CH.
static __forceinline__ __device__ OptixTraversableHandle optixGetGASTraversableHandle();

/// Returns the motion begin time of a GAS (see OptixMotionOptions)
static __forceinline__ __device__ float optixGetGASMotionTimeBegin( OptixTraversableHandle gas );

/// Returns the motion end time of a GAS (see OptixMotionOptions)
static __forceinline__ __device__ float optixGetGASMotionTimeEnd( OptixTraversableHandle gas );

/// Returns the number of motion steps of a GAS (see OptixMotionOptions)
static __forceinline__ __device__ unsigned int optixGetGASMotionStepCount( OptixTraversableHandle gas );

/// Returns the world-to-object transformation matrix resulting from the current active transformation list.
///
/// The cost of this function may be proportional to the size of the transformation list.
static __forceinline__ __device__ void optixGetWorldToObjectTransformMatrix( float m[12] );

/// Returns the object-to-world transformation matrix resulting from the current active transformation list.
///
/// The cost of this function may be proportional to the size of the transformation list.
static __forceinline__ __device__ void optixGetObjectToWorldTransformMatrix( float m[12] );

/// Transforms the point using world-to-object transformation matrix resulting from the current active transformation
/// list.
///
/// The cost of this function may be proportional to the size of the transformation list.
static __forceinline__ __device__ float3 optixTransformPointFromWorldToObjectSpace( float3 point );

/// Transforms the vector using world-to-object transformation matrix resulting from the current active transformation
/// list.
///
/// The cost of this function may be proportional to the size of the transformation list.
static __forceinline__ __device__ float3 optixTransformVectorFromWorldToObjectSpace( float3 vec );

/// Transforms the normal using world-to-object transformation matrix resulting from the current active transformation
/// list.
///
/// The cost of this function may be proportional to the size of the transformation list.
static __forceinline__ __device__ float3 optixTransformNormalFromWorldToObjectSpace( float3 normal );

/// Transforms the point using object-to-world transformation matrix resulting from the current active transformation
/// list.
///
/// The cost of this function may be proportional to the size of the transformation list.
static __forceinline__ __device__ float3 optixTransformPointFromObjectToWorldSpace( float3 point );

/// Transforms the vector using object-to-world transformation matrix resulting from the current active transformation
/// list.
///
/// The cost of this function may be proportional to the size of the transformation list.
static __forceinline__ __device__ float3 optixTransformVectorFromObjectToWorldSpace( float3 vec );

/// Transforms the normal using object-to-world transformation matrix resulting from the current active transformation
/// list.
///
/// The cost of this function may be proportional to the size of the transformation list.
static __forceinline__ __device__ float3 optixTransformNormalFromObjectToWorldSpace( float3 normal );

/// Returns the number of transforms on the current transform list.
///
/// Only available in IS, AH, CH, EX
static __forceinline__ __device__ unsigned int optixGetTransformListSize();

/// Returns the traversable handle for a transform on the current transform list.
///
/// Only available in IS, AH, CH, EX
static __forceinline__ __device__ OptixTraversableHandle optixGetTransformListHandle( unsigned int index );


/// Returns the transform type of a traversable handle from a transform list.
static __forceinline__ __device__ OptixTransformType optixGetTransformTypeFromHandle( OptixTraversableHandle handle );

/// Returns a pointer to a OptixStaticTransform from its traversable handle.
///
/// Returns 0 if the traversable is not of type OPTIX_TRANSFORM_TYPE_STATIC_TRANSFORM.
static __forceinline__ __device__ const OptixStaticTransform* optixGetStaticTransformFromHandle( OptixTraversableHandle handle );

/// Returns a pointer to a OptixSRTMotionTransform from its traversable handle.
///
/// Returns 0 if the traversable is not of type OPTIX_TRANSFORM_TYPE_SRT_MOTION_TRANSFORM.
static __forceinline__ __device__ const OptixSRTMotionTransform* optixGetSRTMotionTransformFromHandle( OptixTraversableHandle handle );

/// Returns a pointer to a OptixMatrixMotionTransform from its traversable handle.
///
/// Returns 0 if the traversable is not of type OPTIX_TRANSFORM_TYPE_MATRIX_MOTION_TRANSFORM.
static __forceinline__ __device__ const OptixMatrixMotionTransform* optixGetMatrixMotionTransformFromHandle( OptixTraversableHandle handle );

/// Returns instanceId from an OptixInstance traversable.
///
/// Returns 0 if the traversable handle does not reference an OptixInstance.
static __forceinline__ __device__ unsigned int optixGetInstanceIdFromHandle( OptixTraversableHandle handle );

/// Returns object-to-world transform from an OptixInstance traversable.
///
/// Returns 0 if the traversable handle does not reference an OptixInstance.
static __forceinline__ __device__ const float4* optixGetInstanceTransformFromHandle( OptixTraversableHandle handle );

/// Returns world-to-object transform from an OptixInstance traversable.
///
/// Returns 0 if the traversable handle does not reference an OptixInstance.
static __forceinline__ __device__ const float4* optixGetInstanceInverseTransformFromHandle( OptixTraversableHandle handle );

/// Reports an intersections (overload without attributes).
///
/// If optixGetRayTmin() <= hitT <= optixGetRayTmax(), the any hit program associated with this intersection program (via the SBT entry) is called.
/// The AH program can do one of three things:
/// 1. call optixIgnoreIntersection - no hit is recorded, optixReportIntersection returns false
/// 2. call optixTerminateRay       -    hit is recorded, optixReportIntersection does not return, no further traversal occurs,
///                                       and the associated closest hit program is called
/// 3. neither                   -    hit is recorded, optixReportIntersection returns true
/// hitKind - Only the 7 least significant bits should be written [0..127].  Any values above 127 are reserved for built in intersection.  The value can be queried with optixGetHitKind() in AH and CH.
///
/// The attributes specified with a0..a7 are available in the AH and CH programs.
/// Note that the attributes available in the CH program correspond to the closest recorded intersection.
/// The number of attributes in registers and memory can be configured in the pipeline.
///
/// \param[in] hitT
/// \param[in] hitKind
static __forceinline__ __device__ bool optixReportIntersection( float hitT, unsigned int hitKind );

/// Reports an intersection (overload with 1 attribute register).
///
/// \see #optixReportIntersection(float,unsigned int)
static __forceinline__ __device__ bool optixReportIntersection( float hitT, unsigned int hitKind, unsigned int a0 );

/// Reports an intersection (overload with 2 attribute registers).
///
/// \see #optixReportIntersection(float,unsigned int)
static __forceinline__ __device__ bool optixReportIntersection( float hitT, unsigned int hitKind, unsigned int a0, unsigned int a1 );

/// Reports an intersection (overload with 3 attribute registers).
///
/// \see #optixReportIntersection(float,unsigned int)
static __forceinline__ __device__ bool optixReportIntersection( float hitT, unsigned int hitKind, unsigned int a0, unsigned int a1, unsigned int a2 );

/// Reports an intersection (overload with 4 attribute registers).
///
/// \see #optixReportIntersection(float,unsigned int)
static __forceinline__ __device__ bool optixReportIntersection( float        hitT,
                                                                unsigned int hitKind,
                                                                unsigned int a0,
                                                                unsigned int a1,
                                                                unsigned int a2,
                                                                unsigned int a3 );

/// Reports an intersection (overload with 5 attribute registers).
///
/// \see #optixReportIntersection(float,unsigned int)
static __forceinline__ __device__ bool optixReportIntersection( float        hitT,
                                                                unsigned int hitKind,
                                                                unsigned int a0,
                                                                unsigned int a1,
                                                                unsigned int a2,
                                                                unsigned int a3,
                                                                unsigned int a4 );

/// Reports an intersection (overload with 6 attribute registers).
///
/// \see #optixReportIntersection(float,unsigned int)
static __forceinline__ __device__ bool optixReportIntersection( float        hitT,
                                                                unsigned int hitKind,
                                                                unsigned int a0,
                                                                unsigned int a1,
                                                                unsigned int a2,
                                                                unsigned int a3,
                                                                unsigned int a4,
                                                                unsigned int a5 );

/// Reports an intersection (overload with 7 attribute registers).
///
/// \see #optixReportIntersection(float,unsigned int)
static __forceinline__ __device__ bool optixReportIntersection( float        hitT,
                                                                unsigned int hitKind,
                                                                unsigned int a0,
                                                                unsigned int a1,
                                                                unsigned int a2,
                                                                unsigned int a3,
                                                                unsigned int a4,
                                                                unsigned int a5,
                                                                unsigned int a6 );

/// Reports an intersection (overload with 8 attribute registers).
///
/// \see #optixReportIntersection(float,unsigned int)
static __forceinline__ __device__ bool optixReportIntersection( float        hitT,
                                                                unsigned int hitKind,
                                                                unsigned int a0,
                                                                unsigned int a1,
                                                                unsigned int a2,
                                                                unsigned int a3,
                                                                unsigned int a4,
                                                                unsigned int a5,
                                                                unsigned int a6,
                                                                unsigned int a7 );

/// Returns the attribute at slot 0.
static __forceinline__ __device__ unsigned int optixGetAttribute_0();
/// Returns the attribute at slot 1.
static __forceinline__ __device__ unsigned int optixGetAttribute_1();
/// Returns the attribute at slot 2.
static __forceinline__ __device__ unsigned int optixGetAttribute_2();
/// Returns the attribute at slot 3.
static __forceinline__ __device__ unsigned int optixGetAttribute_3();
/// Returns the attribute at slot 4.
static __forceinline__ __device__ unsigned int optixGetAttribute_4();
/// Returns the attribute at slot 5.
static __forceinline__ __device__ unsigned int optixGetAttribute_5();
/// Returns the attribute at slot 6.
static __forceinline__ __device__ unsigned int optixGetAttribute_6();
/// Returns the attribute at slot 7.
static __forceinline__ __device__ unsigned int optixGetAttribute_7();

/// Record the hit, stops traversal, and proceeds to CH.
///
/// Available only in AH.
static __forceinline__ __device__ void optixTerminateRay();

/// Discards the hit, and returns control to the calling optixReportIntersection or built-in intersection routine.
///
/// Available only in AH.
static __forceinline__ __device__ void optixIgnoreIntersection();


/// For a given OptixBuildInputTriangleArray the number of primitives is defined as
/// "(OptixBuildInputTriangleArray::indexBuffer == 0) ? OptixBuildInputTriangleArray::numVertices/3 :
///                                                     OptixBuildInputTriangleArray::numIndexTriplets;".
/// For a given OptixBuildInputCustomPrimitiveArray the number of primitives is defined as
/// numAabbs.
///
/// The primitive index returns the index into the array of primitives
/// plus the primitiveIndexOffset.
///
/// In IS and AH this corresponds to the currently intersected primitive.
/// In CH this corresponds to the primitive index of the closest intersected primitive.
static __forceinline__ __device__ unsigned int optixGetPrimitiveIndex();

/// Returns the Sbt GAS index of the primitive associated with the current intersection.
///
/// In IS and AH this corresponds to the currently intersected primitive.
/// In CH this corresponds to the Sbt GAS index of the closest intersected primitive.
/// In EX with exception code OPTIX_EXCEPTION_CODE_TRAVERSAL_INVALID_HIT_SBT corresponds to the sbt index within the hit GAS. Returns zero for all other exceptions.
static __forceinline__ __device__ unsigned int optixGetSbtGASIndex();


/// Returns the OptixInstance::instanceId of the instance within the top level acceleration structure associated with the current intersection.
///
/// When building an acceleration structure using OptixBuildInputInstanceArray each OptixInstance has a user supplied instanceId.
/// OptixInstance objects reference another acceleration structure.  During traversal the acceleration structures are visited top down.
/// In the IS and AH programs the OptixInstance::instanceId corresponding to the most recently visited OptixInstance is returned when calling optixGetInstanceId().
/// In CH optixGetInstanceId() returns the OptixInstance::instanceId when the hit was recorded with optixReportIntersection.
/// In the case where there is no OptixInstance visited, optixGetInstanceId returns ~0u
static __forceinline__ __device__ unsigned int optixGetInstanceId();

/// Returns the zero-based index of the instance within its instance acceleration structure associated with the current intersection.
///
/// In the IS and AH programs the index corresponding to the most recently visited OptixInstance is returned when calling optixGetInstanceIndex().
/// In CH optixGetInstanceIndex() returns the index when the hit was recorded with optixReportIntersection.
/// In the case where there is no OptixInstance visited, optixGetInstanceIndex returns 0
static __forceinline__ __device__ unsigned int optixGetInstanceIndex();

/// Returns the 8 bit hit kind associated with the current hit.
/// 
/// Use optixGetPrimitiveType() to interpret the hit kind.
/// For custom intersections (primitive type OPTIX_PRIMITIVE_TYPE_CUSTOM),
/// this is the 7-bit hitKind passed to optixReportIntersection(). 
/// Hit kinds greater than 127 are reserved for built-in primitives.
///
/// Available only in AH and CH.
static __forceinline__ __device__ unsigned int optixGetHitKind();

/// Function interpreting the result of #optixGetHitKind().
static __forceinline__ __device__ OptixPrimitiveType optixGetPrimitiveType( unsigned int hitKind );

/// Function interpreting the result of #optixGetHitKind().
static __forceinline__ __device__ bool optixIsFrontFaceHit( unsigned int hitKind );

/// Function interpreting the result of #optixGetHitKind().
static __forceinline__ __device__ bool optixIsBackFaceHit( unsigned int hitKind );

/// Function interpreting the hit kind associated with the current optixReportIntersection.
static __forceinline__ __device__ OptixPrimitiveType optixGetPrimitiveType();

/// Function interpreting the hit kind associated with the current optixReportIntersection.
static __forceinline__ __device__ bool optixIsFrontFaceHit();

/// Function interpreting the hit kind associated with the current optixReportIntersection.
static __forceinline__ __device__ bool optixIsBackFaceHit();

/// Convenience function interpreting the result of #optixGetHitKind().
static __forceinline__ __device__ bool optixIsTriangleHit();

/// Convenience function interpreting the result of #optixGetHitKind().
static __forceinline__ __device__ bool optixIsTriangleFrontFaceHit();

/// Convenience function interpreting the result of #optixGetHitKind().
static __forceinline__ __device__ bool optixIsTriangleBackFaceHit();

/// Convenience function that returns the first two attributes as floats.
///
/// When using OptixBuildInputTriangleArray objects, during intersection the barycentric
/// coordinates are stored into the first two attribute registers.
static __forceinline__ __device__ float2 optixGetTriangleBarycentrics();

/// Convenience function that returns the curve parameter.
///
/// When using OptixBuildInputCurveArray objects, during intersection the curve parameter
/// is stored into the first attribute register.
static __forceinline__ __device__ float optixGetCurveParameter();

/// Available in any program, it returns the current launch index within the launch dimensions specified by optixLaunch on the host.
///
/// The raygen program is typically only launched once per launch index.
static __forceinline__ __device__ uint3 optixGetLaunchIndex();

/// Available in any program, it returns the dimensions of the current launch specified by optixLaunch on the host.
static __forceinline__ __device__ uint3 optixGetLaunchDimensions();

/// Returns the generic memory space pointer to the data region (past the header) of the currently active SBT record corresponding to the current program.
static __forceinline__ __device__ CUdeviceptr optixGetSbtDataPointer();

/// Throws a user exception with the given exception code (overload without exception details).
///
/// The exception code must be in the range from 0 to 2^30 - 1. Up to 8 optional exception details can be passed. They
/// can be queried in the EX program using optixGetExceptionDetail_0() to ..._8().
///
/// The exception details must not be used to encode pointers to the stack since the current stack is not preserved in
/// the EX program.
///
/// Not available in EX.
///
/// \param[in] exceptionCode The exception code to be thrown.
static __forceinline__ __device__ void optixThrowException( int exceptionCode );

/// Throws a user exception with the given exception code (overload with 1 exception detail).
///
/// \see #optixThrowException(int)
static __forceinline__ __device__ void optixThrowException( int exceptionCode, unsigned int exceptionDetail0 );

/// Throws a user exception with the given exception code (overload with 2 exception details).
///
/// \see #optixThrowException(int)
static __forceinline__ __device__ void optixThrowException( int exceptionCode,
                                                            unsigned int exceptionDetail0,
                                                            unsigned int exceptionDetail1 );

/// Throws a user exception with the given exception code (overload with 3 exception details).
///
/// \see #optixThrowException(int)
static __forceinline__ __device__ void optixThrowException( int exceptionCode,
                                                            unsigned int exceptionDetail0,
                                                            unsigned int exceptionDetail1,
                                                            unsigned int exceptionDetail2 );

/// Throws a user exception with the given exception code (overload with 4 exception details).
///
/// \see #optixThrowException(int)
static __forceinline__ __device__ void optixThrowException( int exceptionCode,
                                                            unsigned int exceptionDetail0,
                                                            unsigned int exceptionDetail1,
                                                            unsigned int exceptionDetail2,
                                                            unsigned int exceptionDetail3 );

/// Throws a user exception with the given exception code (overload with 5 exception details).
///
/// \see #optixThrowException(int)
static __forceinline__ __device__ void optixThrowException( int exceptionCode,
                                                            unsigned int exceptionDetail0,
                                                            unsigned int exceptionDetail1,
                                                            unsigned int exceptionDetail2,
                                                            unsigned int exceptionDetail3,
                                                            unsigned int exceptionDetail4 );

/// Throws a user exception with the given exception code (overload with 6 exception details).
///
/// \see #optixThrowException(int)
static __forceinline__ __device__ void optixThrowException( int exceptionCode,
                                                            unsigned int exceptionDetail0,
                                                            unsigned int exceptionDetail1,
                                                            unsigned int exceptionDetail2,
                                                            unsigned int exceptionDetail3,
                                                            unsigned int exceptionDetail4,
                                                            unsigned int exceptionDetail5 );

/// Throws a user exception with the given exception code (overload with 7 exception details).
///
/// \see #optixThrowException(int)
static __forceinline__ __device__ void optixThrowException( int exceptionCode,
                                                            unsigned int exceptionDetail0,
                                                            unsigned int exceptionDetail1,
                                                            unsigned int exceptionDetail2,
                                                            unsigned int exceptionDetail3,
                                                            unsigned int exceptionDetail4,
                                                            unsigned int exceptionDetail5,
                                                            unsigned int exceptionDetail6 );

/// Throws a user exception with the given exception code (overload with 8 exception details).
///
/// \see #optixThrowException(int)
static __forceinline__ __device__ void optixThrowException( int exceptionCode,
                                                            unsigned int exceptionDetail0,
                                                            unsigned int exceptionDetail1,
                                                            unsigned int exceptionDetail2,
                                                            unsigned int exceptionDetail3,
                                                            unsigned int exceptionDetail4,
                                                            unsigned int exceptionDetail5,
                                                            unsigned int exceptionDetail6,
                                                            unsigned int exceptionDetail7 );

/// Returns the exception code.
///
/// Only available in EX.
static __forceinline__ __device__ int optixGetExceptionCode();

/// Returns the 32-bit exception detail at slot 0.
///
/// The behavior is undefined if the exception is not a user exception, or the used overload #optixThrowException() did
/// not provide the queried exception detail.
///
/// Only available in EX.
static __forceinline__ __device__ unsigned int optixGetExceptionDetail_0();

/// Returns the 32-bit exception detail at slot 1.
///
/// \see #optixGetExceptionDetail_0()
static __forceinline__ __device__ unsigned int optixGetExceptionDetail_1();

/// Returns the 32-bit exception detail at slot 2.
///
/// \see #optixGetExceptionDetail_0()
static __forceinline__ __device__ unsigned int optixGetExceptionDetail_2();

/// Returns the 32-bit exception detail at slot 3.
///
/// \see #optixGetExceptionDetail_0()
static __forceinline__ __device__ unsigned int optixGetExceptionDetail_3();

/// Returns the 32-bit exception detail at slot 4.
///
/// \see #optixGetExceptionDetail_0()
static __forceinline__ __device__ unsigned int optixGetExceptionDetail_4();

/// Returns the 32-bit exception detail at slot 5.
///
/// \see #optixGetExceptionDetail_0()
static __forceinline__ __device__ unsigned int optixGetExceptionDetail_5();

/// Returns the 32-bit exception detail at slot 6.
///
/// \see #optixGetExceptionDetail_0()
static __forceinline__ __device__ unsigned int optixGetExceptionDetail_6();

/// Returns the 32-bit exception detail at slot 7.
///
/// \see #optixGetExceptionDetail_0()
static __forceinline__ __device__ unsigned int optixGetExceptionDetail_7();

/// Returns the invalid traversable handle for exceptions with exception code OPTIX_EXCEPTION_CODE_TRAVERSAL_INVALID_TRAVERSABLE. 
/// 
/// Returns zero for all other exception codes. 
/// 
/// Only available in EX.
static __forceinline__ __device__ OptixTraversableHandle optixGetExceptionInvalidTraversable();

/// Returns the invalid sbt offset for exceptions with exception code OPTIX_EXCEPTION_CODE_TRAVERSAL_INVALID_MISS_SBT and OPTIX_EXCEPTION_CODE_TRAVERSAL_INVALID_HIT_SBT. 
/// 
/// Returns zero for all other exception codes. 
/// 
/// Only available in EX.
static __forceinline__ __device__ int optixGetExceptionInvalidSbtOffset();

/// Returns the invalid ray for exceptions with exception code OPTIX_EXCEPTION_CODE_INVALID_RAY.
/// Exceptions of type OPTIX_EXCEPTION_CODE_INVALID_RAY are thrown when one or more values that were
/// passed into optixTrace are either inf or nan.
///
/// OptixInvalidRayExceptionDetails::rayTime will always be 0 if OptixPipelineCompileOptions::usesMotionBlur is 0.
/// Values in the returned struct are all zero for all other exception codes.
/// 
/// Only available in EX.
static __forceinline__ __device__ OptixInvalidRayExceptionDetails optixGetExceptionInvalidRay();

/// Returns information about an exception with code OPTIX_EXCEPTION_CODE_CALLABLE_PARAMETER_MISMATCH.
/// 
/// Exceptions of type OPTIX_EXCEPTION_CODE_CALLABLE_PARAMETER_MISMATCH are called when the number of
/// arguments that were passed into a call to optixDirectCall or optixContinuationCall does not match
/// the number of parameters of the callable that is called.
/// Note that the parameters are packed by OptiX into individual 32 bit values, so the number of
/// expected and passed values may not correspond to the number of arguments passed into optixDirectCall
/// or optixContinuationCall.
/// 
/// Values in the returned struct are all zero for all other exception codes.
/// 
/// Only available in EX.
static __forceinline__ __device__ OptixParameterMismatchExceptionDetails optixGetExceptionParameterMismatch();

/// Returns a string that includes information about the source location that caused the current exception.
///
/// The source location is only available for exceptions of type OPTIX_EXCEPTION_CODE_CALLABLE_PARAMETER_MISMATCH,
/// OPTIX_EXCEPTION_CODE_UNSUPPORTED_PRIMITIVE_TYPE, OPTIX_EXCEPTION_CODE_INVALID_RAY, and for user exceptions.
/// Line information needs to be present in the input PTX and OptixModuleCompileOptions::debugLevel
/// may not be set to OPTIX_COMPILE_DEBUG_LEVEL_NONE.
/// 
/// Returns a NULL pointer if no line information is available.
/// 
/// Only available in EX.
static __forceinline__ __device__ char* optixGetExceptionLineInfo();

/// Creates a call to the direct callable program at the specified SBT entry.
/// 
/// This will call the program that was specified in the OptixProgramGroupCallables::entryFunctionNameDC in the
/// module specified by OptixProgramGroupCallables::moduleDC.
/// The address of the SBT entry is calculated by OptixShaderBindingTable::callablesRecordBase + ( OptixShaderBindingTable::callablesRecordStrideInBytes * sbtIndex ).
/// 
/// Behavior is undefined if there is no direct callable program at the specified SBT entry.
/// 
/// Behavior is undefined if the number of arguments that are being passed in does not match the number of
/// parameters expected by the program that is called. In that case an exception of type OPTIX_EXCEPTION_CODE_CALLABLE_PARAMETER_MISMATCH 
/// will be thrown if OPTIX_EXCEPTION_FLAG_DEBUG was specified for the OptixPipelineCompileOptions::exceptionFlags.
///
/// \param[in] sbtIndex The offset of the SBT entry of the direct callable program to call relative to OptixShaderBindingTable::callablesRecordBase.
/// \param[in] args The arguments to pass to the direct callable program.
template <typename ReturnT, typename... ArgTypes>
static __forceinline__ __device__ ReturnT optixDirectCall( unsigned int sbtIndex, ArgTypes... args );


/// Creates a call to the continuation callable program at the specified SBT entry.
/// 
/// This will call the program that was specified in the OptixProgramGroupCallables::entryFunctionNameCC in the
/// module specified by OptixProgramGroupCallables::moduleCC.
/// The address of the SBT entry is calculated by OptixShaderBindingTable::callablesRecordBase + ( OptixShaderBindingTable::callablesRecordStrideInBytes * sbtIndex ).
/// As opposed to direct callable programs, continuation callable programs are allowed to call optixTrace recursively.
/// 
/// Behavior is undefined if there is no continuation callable program at the specified SBT entry.
/// 
/// Behavior is undefined if the number of arguments that are being passed in does not match the number of
/// parameters expected by the program that is called. In that case an exception of type OPTIX_EXCEPTION_CODE_CALLABLE_PARAMETER_MISMATCH 
/// will be thrown if OPTIX_EXCEPTION_FLAG_DEBUG was specified for the OptixPipelineCompileOptions::exceptionFlags.
///
/// \param[in] sbtIndex The offset of the SBT entry of the continuation callable program to call relative to OptixShaderBindingTable::callablesRecordBase.
/// \param[in] args The arguments to pass to the continuation callable program.
template <typename ReturnT, typename... ArgTypes>
static __forceinline__ __device__ ReturnT optixContinuationCall( unsigned int sbtIndex, ArgTypes... args );

/*@}*/  // end group optix_device_api

#include "internal/optix_7_device_impl.h"

#endif  // __optix_optix_7_device_h__
