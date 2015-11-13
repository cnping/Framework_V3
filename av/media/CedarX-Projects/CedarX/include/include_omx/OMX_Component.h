/*
 * Copyright (c) 2005 The Khronos Group Inc. 
 * 
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject
 * to the following conditions: 
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software. 
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 
 *
 */

/** OMX_Component.h
 *  The OMX_Component header file contains the definitions used to define
 *  the public interface of a component.  This header file is intended to
 *  be used by both the application and the component.
 */

#ifndef OMX_Component_h
#define OMX_Component_h

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */



/* Each OMX header must include all required header files to allow the
 *  header to compile without errors.  The includes below are required
 *  for this header file to compile successfully 
 */
#include <OMX_Types.h>
//#include <OMX_Core.h>
#include <OMX_Audio.h>
#include <OMX_Video.h>
#include <OMX_Image.h>
#include <OMX_Other.h>

typedef enum OMX_PORTDOMAINTYPE { 
    OMX_PortDomainAudio, 
    OMX_PortDomainVideo, 
    OMX_PortDomainImage, 
    OMX_PortDomainOther,
    OMX_PortDomainSubtitle,

    OMX_PortDomainMax = 0x7ffffff
} OMX_PORTDOMAINTYPE;

typedef enum OMX_COMPONENTNAMETYPE {
	OMX_ComponentDemux,
	OMX_ComponentAudio,
	OMX_ComponentVideo,
	OMX_ComponentClock,
	OMX_ComponentAudioRender,
	OMX_ComponentVideoRender,
	OMX_ComponentSubtitle,
	
	// star add
	OMX_ComponentVideoSource,
	OMX_ComponentAudioSource,
	OMX_ComponentVideoEnc,
	OMX_ComponentAudioEnc,
	OMX_ComponentAudioTransmit,
	OMX_ComponentRecRender,
	OMX_ComponentOther,
} OMX_COMPONENTNAMETYPE;

typedef struct OMX_PARAM_PORTDEFINITIONTYPE {
    OMX_U32 nSize;                 /**< Size of the structure in bytes */
    OMX_VERSIONTYPE nVersion;      /**< OMX specification version information */
    OMX_U32 nPortIndex;            /**< Port number the structure applies to */
    OMX_DIRTYPE eDir;              /**< Direction (input or output) of this port */
    OMX_U32 nBufferCountActual;    /**< The actual number of buffers allocated on this port */
    OMX_U32 nBufferCountMin;       /**< The minimum number of buffers this port requires */
    OMX_U32 nBufferSize;           /**< Size, in bytes, for buffers to be used for this channel */
    OMX_BOOL bEnabled;             /**< Ports default to enabled and are enabled/disabled by
                                        OMX_CommandPortEnable/OMX_CommandPortDisable.
                                        When disabled a port is unpopulated. A disabled port
                                        is not populated with buffers on a transition to IDLE. */
    OMX_BOOL bPopulated;           /**< Port is populated with all of its buffers as indicated by
                                        nBufferCountActual. A disabled port is always unpopulated. 
                                        An enabled port is populated on a transition to OMX_StateIdle
                                        and unpopulated on a transition to loaded. */
    OMX_PORTDOMAINTYPE eDomain;    /**< Domain of the port. Determines the contents of metadata below. */
    union {
    	OMX_AUDIO_PORTDEFINITIONTYPE audio;
	    OMX_VIDEO_PORTDEFINITIONTYPE video;
	    OMX_IMAGE_PORTDEFINITIONTYPE image;
	    OMX_OTHER_PORTDEFINITIONTYPE other;
    } format;

    OMX_TIME_CONFIG_TIMESTAMPTYPE sTimeStamp; //for clock component
    void* pVendorInfo; //add by vendor
} OMX_PARAM_PORTDEFINITIONTYPE;


/** The OMX_HANDLETYPE structure defines the component handle.  The component 
 *  handle is used to access all of the component's public methods and also
 *  contains pointers to the component's private data area.  The component
 *  handle is initialized by the OMX core (with help from the component)
 *  during the process of loading the component.  After the component is
 *  successfully loaded, the application can safely access any of the
 *  component's public functions (although some may return an error because
 *  the state is inappropriate for the access).
 */
typedef struct OMX_COMPONENTTYPE
{
    /** The size of this structure, in bytes.  It is the responsibility
        of the allocator of this structure to fill in this value.  Since
        this structure is allocated by the GetHandle function, this
        function will fill in this value. */
    OMX_U32 nSize;

    /** nVersion is the version of the OMX specification that the structure 
        is built against.  It is the responsibility of the creator of this 
        structure to initialize this value and every user of this structure 
        should verify that it knows how to use the exact version of 
        this structure found herein. */
    OMX_VERSIONTYPE nVersion;

    OMX_COMPONENTNAMETYPE eCompName;

    /** pComponentPrivate is a pointer to the component private data area.  
        This member is allocated and initialized by the component when the 
        component is first loaded.  The application should not access this 
        data area. */
    OMX_PTR pComponentPrivate;

    /** pApplicationPrivate is a pointer that is a parameter to the 
        OMX_GetHandle method, and contains an application private value 
        provided by the IL client.  This application private data is 
        returned to the IL Client by OMX in all callbacks */
    OMX_PTR pApplicationPrivate;

    /** refer to OMX_GetComponentVersion in OMX_core.h or the OMX IL 
        specification for details on the GetComponentVersion method.
     */
    OMX_ERRORTYPE (*GetComponentVersion)(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_OUT OMX_STRING pComponentName,
            OMX_OUT OMX_VERSIONTYPE* pComponentVersion,
            OMX_OUT OMX_VERSIONTYPE* pSpecVersion,
            OMX_OUT OMX_UUIDTYPE* pComponentUUID);

    /** refer to OMX_SendCommand in OMX_core.h or the OMX IL 
        specification for details on the SendCommand method.
     */
    OMX_ERRORTYPE (*SendCommand)(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_IN  OMX_COMMANDTYPE Cmd,
            OMX_IN  OMX_U32 nParam1,
            OMX_IN  OMX_PTR pCmdData);

    /** refer to OMX_GetParameter in OMX_core.h or the OMX IL 
        specification for details on the GetParameter method.
     */
    OMX_ERRORTYPE (*GetParameter)(
            OMX_IN  OMX_HANDLETYPE hComponent, 
            OMX_IN  OMX_INDEXTYPE nParamIndex,  
            OMX_INOUT OMX_PTR ComponentParameterStructure);


    /** refer to OMX_SetParameter in OMX_core.h or the OMX IL 
        specification for details on the SetParameter method.
     */
    OMX_ERRORTYPE (*SetParameter)(
            OMX_IN  OMX_HANDLETYPE hComponent, 
            OMX_IN  OMX_INDEXTYPE nIndex,
            OMX_IN  OMX_PTR ComponentParameterStructure);


    /** refer to OMX_GetConfig in OMX_core.h or the OMX IL 
        specification for details on the GetConfig method.
     */
    OMX_ERRORTYPE (*GetConfig)(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_IN  OMX_INDEXTYPE nIndex, 
            OMX_INOUT OMX_PTR pComponentConfigStructure);


    /** refer to OMX_SetConfig in OMX_core.h or the OMX IL 
        specification for details on the SetConfig method.
     */
    OMX_ERRORTYPE (*SetConfig)(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_IN  OMX_INDEXTYPE nIndex, 
            OMX_IN  OMX_PTR pComponentConfigStructure);


    /** refer to OMX_GetExtensionIndex in OMX_core.h or the OMX IL 
        specification for details on the GetExtensionIndex method.
     */
    OMX_ERRORTYPE (*GetExtensionIndex)(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_IN  OMX_STRING cParameterName,
            OMX_OUT OMX_INDEXTYPE* pIndexType);


    /** refer to OMX_GetState in OMX_core.h or the OMX IL 
        specification for details on the GetState method.
     */
    OMX_ERRORTYPE (*GetState)(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_OUT OMX_STATETYPE* pState);

    
    /** The ComponentTunnelRequest method will interact with another OMX
        component to determine if tunneling is possible and to setup the
        tunneling.  The return codes for this method can be used to 
        determine if tunneling is not possible, or if tunneling is not
        supported.  
        
        Base profile components (i.e. non-interop) do not support this
        method and should return OMX_ErrorNotImplemented 

        The interop profile component MUST support tunneling to another 
        interop profile component with a compatible port parameters.  
        A component may also support proprietary communication.
        
        If proprietary communication is supported the negotiation of 
        proprietary communication is done outside of OMX in a vendor 
        specific way. It is only required that the proper result be 
        returned and the details of how the setup is done is left 
        to the component implementation.  
    
        When this method is invoked when nPort in an output port, the
        component will:
        1.  Populate the pTunnelSetup structure with the output port's 
            requirements and constraints for the tunnel.

        When this method is invoked when nPort in an input port, the
        component will:
        1.  Query the necessary parameters from the output port to 
            determine if the ports are compatible for tunneling
        2.  If the ports are compatible, the component should store
            the tunnel step provided by the output port
        3.  Determine which port (either input or output) is the buffer
            supplier, and call OMX_SetParameter on the output port to
            indicate this selection.
        
        The component will return from this call within 5 msec.
    
        @param [in] hComp
            Handle of the component to be accessed.  This is the component
            handle returned by the call to the OMX_GetHandle method.
        @param [in] nPort
            nPort is used to select the port on the component to be used
            for tunneling.
        @param [in] hTunneledComp
            Handle of the component to tunnel with.  This is the component 
            handle returned by the call to the OMX_GetHandle method.  When
            this parameter is 0x0 the component should setup the port for
            communication with the application / IL Client.
        @param [in] nPortOutput
            nPortOutput is used indicate the port the component should
            tunnel with.
        @param [in] pTunnelSetup
            Pointer to the tunnel setup structure.  When nPort is an output port
            the component should populate the fields of this structure.  When
            When nPort is an input port the component should review the setup
            provided by the component with the output port.
        @return OMX_ERRORTYPE
            If the command successfully executes, the return code will be
            OMX_NoError.  Otherwise the appropriate OMX error will be returned.
    */

    OMX_ERRORTYPE (*ComponentTunnelRequest)(
        OMX_IN  OMX_HANDLETYPE hComp,
        OMX_IN  OMX_U32 nPort,
        OMX_IN  OMX_HANDLETYPE hTunneledComp,
        OMX_IN  OMX_U32 nTunneledPort,
        OMX_INOUT  OMX_TUNNELSETUPTYPE* pTunnelSetup); 

    /** refer to OMX_UseBuffer in OMX_core.h or the OMX IL 
        specification for details on the UseBuffer method.
     */
    OMX_ERRORTYPE (*UseBuffer)(
            OMX_IN OMX_HANDLETYPE hComponent,
            OMX_INOUT OMX_BUFFERHEADERTYPE** ppBufferHdr,
            OMX_IN OMX_U32 nPortIndex,
            OMX_IN OMX_PTR pAppPrivate,
            OMX_IN OMX_U32 nSizeBytes,
            OMX_IN OMX_U8* pBuffer);

    /** refer to OMX_AllocateBuffer in OMX_core.h or the OMX IL 
        specification for details on the AllocateBuffer method.
     */
    OMX_ERRORTYPE (*AllocateBuffer)(
            OMX_IN OMX_HANDLETYPE hComponent,
            OMX_INOUT OMX_BUFFERHEADERTYPE** ppBuffer,
            OMX_IN OMX_U32 nPortIndex,
            OMX_IN OMX_PTR pAppPrivate,
            OMX_IN OMX_U32 nSizeBytes);

    /** refer to OMX_FreeBuffer in OMX_core.h or the OMX IL 
        specification for details on the FreeBuffer method.
     */
    OMX_ERRORTYPE (*FreeBuffer)(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_IN  OMX_U32 nPortIndex,
            OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer);

	OMX_ERRORTYPE (*RequstBuffer)(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_IN  OMX_U32 nPortIndex,
            OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer);

	OMX_ERRORTYPE (*ReleaseBuffer)(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_IN  OMX_U32 nPortIndex,
            OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer);

	OMX_ERRORTYPE (*QueryBufferState)(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_IN  OMX_U32 nPortIndex,
            OMX_IN  OMX_BUFFERSTATE* pBufferState);
	
    /** refer to OMX_EmptyThisBuffer in OMX_core.h or the OMX IL 
        specification for details on the EmptyThisBuffer method.
     */
    OMX_ERRORTYPE (*EmptyThisBuffer)(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer);

    /** refer to OMX_FillThisBuffer in OMX_core.h or the OMX IL 
        specification for details on the FillThisBuffer method.
     */
    OMX_ERRORTYPE (*FillThisBuffer)(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer);
	
    /** The SetCallbacks method is used by the core to specify the callback
        structure from the application to the component.  This is a blocking
        call.  The component will return from this call within 5 msec.
        @param [in] hComponent
            Handle of the component to be accessed.  This is the component
            handle returned by the call to the GetHandle function.
        @param [in] pCallbacks
            pointer to an OMX_CALLBACKTYPE structure used to provide the 
            callback information to the component
        @param [in] pAppData
            pointer to an application defined value.  It is anticipated that 
            the application will pass a pointer to a data structure or a "this
            pointer" in this area to allow the callback (in the application)
            to determine the context of the call
        @return OMX_ERRORTYPE
            If the command successfully executes, the return code will be
            OMX_NoError.  Otherwise the appropriate OMX error will be returned.
     */
    OMX_ERRORTYPE (*SetCallbacks)(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_IN  OMX_CALLBACKTYPE* pCallbacks, 
            OMX_IN  OMX_PTR pAppData);

    /** ComponentDeInit method is used to deinitialize the component
        providing a means to free any resources allocated at component
        initialization.  NOTE:  After this call the component handle is
        not valid for further use.
        @param [in] hComponent
            Handle of the component to be accessed.  This is the component
            handle returned by the call to the GetHandle function.
        @return OMX_ERRORTYPE
            If the command successfully executes, the return code will be
            OMX_NoError.  Otherwise the appropriate OMX error will be returned.
     */
    OMX_ERRORTYPE (*ComponentDeInit)(
            OMX_IN  OMX_HANDLETYPE hComponent);

} OMX_COMPONENTTYPE;

typedef enum OMX_TUNNEL_TYPE{
	TUNNEL_TYPE_COMMON = 0x100,
	TUNNEL_TYPE_CLOCK ,
}OMX_TUNNEL_TYPE;

typedef struct OMX_TUNNELINFOTYPE{
	OMX_U32 nPortIndex;
	OMX_HANDLETYPE hTunnel;
	OMX_U32 nTunnelPortIndex;
	OMX_TUNNEL_TYPE eTunnelType;
}OMX_TUNNELINFOTYPE;

typedef struct OMX_PORTSCALLBACKTYPE{
	OMX_U32 nPortIndex;
	OMX_HANDLETYPE hTunnelComponent;
	OMX_ERRORTYPE (*EventHandler)(
					OMX_IN OMX_HANDLETYPE hComponent,
					OMX_IN OMX_PTR pAppData,
					OMX_IN OMX_EVENTTYPE eEvent,
					OMX_IN OMX_U32 nData1,
					OMX_IN OMX_U32 nData2,
					OMX_IN OMX_PTR pEventData);
}OMX_PORTSCALLBACKTYPE;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
/* File EOF */
