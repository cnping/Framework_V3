#ifndef ONVIF_IPC_H
#define ONVIF_IPC_H

#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

#define VideoEncoderToken_MainStream "MainStream"
#define VideoEncoderToken_SubStream "SubStream"
#define VideoEncoderToken_JPEG "jpeg"	//��ͼ


//========================================================================================
//
//					IPC��Onvif��ʼ������
//
//========================================================================================
//��ʼ��ONVIF��IPC��
void * init_onvif(void * cookies);
//========================================================================================
//
//					IPC��Onvif��������
//
//========================================================================================
//����Onvif���񣬴˺���Ϊ����Onvif�������㣬������Ҫע���йػص�����
int start_onvif(void * ovf_env);
//ֹͣOnvif����
void stop_onvif(void * ovf_env);
//����Onvif
void destroy_onvif(void *ovf_env);
//========================================================================================
//
//					*IPC��IP��ַ����*<�ӿ�������>
//
//========================================================================================
//*****����IPC��IP��ַ,������ip��ַ�����ʮ���Ƹ�ʽ�ַ���
//int SetIPtoIPC(char * ip);
//========================================================================================
//
//					*��ȡIPC��IP��ַ*
//
//========================================================================================
//��ȡIPC��IP��ַ����������ȡ��ip
//				   ����ֵ���ɹ�����0��ʧ�ܷ���-1
typedef int (*GetIPfromIPC)(void *cookies, char * ip);
//ע��ص��������������ص�������
void registerGetIPfromIPC(void * ovf_env, GetIPfromIPC _getIPfromIPC);
//========================================================================================
//
//					��ȡ�豸��Ϣ�ĵ��ýӿ�
//
//========================================================================================
//*****��ȡ�豸�����̣�����������豸�������ַ���
typedef int(*GetDeviceInformationResponse_Manufacturer)(void *cookies, char *Manufacturer);
//ע��ص��������������ص�������
void registerGetDeviceInformationResponse_Manufacturer(void * ovf_env, GetDeviceInformationResponse_Manufacturer _getDeviceInformationResponse_Manufacturer);
//========================================================================================
//*****��ȡ�豸�ͺţ�����������豸�ͺ��ַ���
typedef int (*GetDeviceInformationResponse_Model)(void *cookies, char *Model);
//ע��ص��������������ص�������
void registerGetDeviceInformationResponse_Model(void * ovf_env, GetDeviceInformationResponse_Model _getDeviceInformationResponse_Model);
//========================================================================================
//*****��ȡ�豸�̼��汾������������豸�̼��汾�ַ���
typedef int (*GetDeviceInformationResponse_FirmwareVersion)(void *cookies, char *FirmwareVersion);
//ע��ص��������������ص�������
void registerGetDeviceInformationResponse_FirmwareVersion(void * ovf_env, GetDeviceInformationResponse_FirmwareVersion _getDeviceInformationResponse_FirmwareVersion);
//========================================================================================
//*****��ȡ�豸���кţ�����������豸���к��ַ���
typedef int (*GetDeviceInformationResponse_SerialNumber)(void *cookies, char *SerialNumber);
//ע��ص��������������ص�������
void registerGetDeviceInformationResponse_SerialNumber(void * ovf_env, GetDeviceInformationResponse_SerialNumber _getDeviceInformationResponse_SerialNumber);
//========================================================================================
//*****��ȡ�豸Ӳ��ID������������豸Ӳ��ID�ַ���
typedef int (*GetDeviceInformationResponse_HardwareId)(void *cookies, char *HardwareId);
//ע��ص��������������ص�������
void registerGetDeviceInformationResponse_HardwareId(void * ovf_env, GetDeviceInformationResponse_HardwareId _getDeviceInformationResponse_HardwareId);
//========================================================================================
//
//					��ȡ/�����豸����ʱ��ĵ��ýӿ�
//
//========================================================================================
//��������ʱ�����ͣ��������ֶ�Ϊ0��NTPУʱΪ1
typedef	int (*SetSystemDateAndTime_DateTimeType)(void *cookies, int DateTimeType);
//ע��ص��������������ص�������
void registerSetSystemDateAndTime_DateTimeType(void * ovf_env, SetSystemDateAndTime_DateTimeType _setSystemDateAndTime_DateTimeType);
//========================================================================================
//������ʱ�ƣ���������ЧΪ0����ЧΪ1
typedef int (*SetSystemDateAndTime_DaylightSavings)(void *cookies, int DaylightSavings);
//ע��ص��������������ص�������
void registerSetSystemDateAndTime_DaylightSavings(void * ovf_env, SetSystemDateAndTime_DaylightSavings _setSystemDateAndTime_DaylightSavings);
//========================================================================================
//����ʱ�������������ʱ���ַ���
typedef int (*SetSystemDateAndTime_TimeZone)(void *cookies, char *TimeZone);
//ע��ص��������������ص�������
void registerSetSystemDateAndTime_TimeZone(void * ovf_env, SetSystemDateAndTime_TimeZone  _setSystemDateAndTime_TimeZone);
//========================================================================================
//*****����UTCʱ�䣬���������struct tm�ṹ��
typedef int (*SetSystemDateAndTime_UTCDateTime)(void *cookies, struct tm *UTCDateTime);
//ע��ص��������������ص�������
void registerSetSystemDateAndTime_UTCDateTime(void * ovf_env, SetSystemDateAndTime_UTCDateTime _setSystemDateAndTime_UTCDateTime);
//========================================================================================
//��ȡ����ʱ�����ͣ��������ֶ�Ϊ0��NTPУʱΪ1
typedef int (*GetSystemDateAndTimeResponse_DateTimeType)(void *cookies, int *DateTimeType);
//ע��ص��������������ص�������
void registerGetSystemDateAndTimeResponse_DateTimeType(void * ovf_env, GetSystemDateAndTimeResponse_DateTimeType _getSystemDateAndTimeResponse_DateTimeType);
//========================================================================================
//��ȡ��ʱ�ƣ���������ЧΪ0����ЧΪ1
typedef int (*GetSystemDateAndTimeResponse_DaylightSavings)(void *cookies, int *DaylightSavings);
//ע��ص��������������ص�������
void registerGetSystemDateAndTimeResponse_DaylightSavings(void * ovf_env, GetSystemDateAndTimeResponse_DaylightSavings _getSystemDateAndTimeResponse_DaylightSavings);
//========================================================================================
//��ȡʱ�������������ʱ���ַ���
typedef int (*GetSystemDateAndTimeResponse_TimeZone)(void *cookies, char *TimeZone);
//ע��ص��������������ص�������
void registerGetSystemDateAndTimeResponse_TimeZone(void * ovf_env, GetSystemDateAndTimeResponse_TimeZone _getSystemDateAndTimeResponse_TimeZone);
//========================================================================================
//*****��ȡUTCʱ�䣬���������struct tm�ṹ��
typedef int (*GetSystemDateAndTimeResponse_UTCDateTime)(void *cookies, struct tm *UTCDateTime);
//ע��ص��������������ص�������
void registerGetSystemDateAndTimeResponse_UTCDateTime(void * ovf_env, GetSystemDateAndTimeResponse_UTCDateTime _getSystemDateAndTimeResponse_UTCDateTime);
//========================================================================================
//*****��ȡ����ʱ�䣬���������struct tm�ṹ��
typedef int (*GetSystemDateAndTimeResponse_LocalDateTime)(void *cookies, struct tm *LocalDateTime);
//ע��ص��������������ص�������
void registerGetSystemDateAndTimeResponse_LocalDateTime(void * ovf_env, GetSystemDateAndTimeResponse_LocalDateTime _getSystemDateAndTimeResponse_LocalDateTime);
//========================================================================================
//
//					��ȡ�豸�����ĵ��ýӿ�
//
//========================================================================================
//��ȡ�豸������������������RuleSupport��0Ϊ��֧�֣�1Ϊ֧�֣���AnalyticsModuleSupport��0Ϊ��֧�֣�1Ϊ֧�֣�  ע�����ֻ��ȷ���豸�з�����Ƶ�Ĺ��ܣ�����ȷ������Ĳ�����������ã�����������Ϊ0��֮���������޸ģ�����ӿڵ����ͬ����������
typedef int (*GetCapabilitiesResponse_Analytics)(void *cookies, int *RuleSupport, int *AnalyticsModuleSupport);
//ע��ص��������������ص�������
void registerGetCapabilitiesResponse_Analytics(void * ovf_env, GetCapabilitiesResponse_Analytics _getCapabilitiesResponse_Analytics);
//========================================================================================
//��ȡ�豸����������������IPFilter,  ZeroConfiguration, 	 IPVersion6,  DynDNS,  Dot11Configuration(0Ϊ��֧�֣�1Ϊ֧�֣�
typedef int (*GetCapabilitiesResponse_Device_Network)(void *cookies, int *IPFilter,	int *ZeroConfiguration, int *IPVersion6, int *DynDNS, int *Dot11Configuration);
//ע��ص��������������ص�������
void registerGetCapabilitiesResponse_Device_Network(void * ovf_env, GetCapabilitiesResponse_Device_Network _getCapabilitiesResponse_Device_Network);
//========================================================================================
//��ȡ�豸ϵͳ������������RemoteDiscovery��SystemBackup��SystemLogging��FirmwareUpgrade(0Ϊ��֧�֣�1Ϊ֧�֣�
typedef int (*GetCapabilitiesResponse_Device_System)(void *cookies, int *RemoteDiscovery,int *SystemBackup,int *SystemLogging,int *FirmwareUpgrade);
//ע��ص��������������ص�������
void registerGetCapabilitiesResponse_Device_System(void * ovf_env, GetCapabilitiesResponse_Device_System _getCapabilitiesResponse_Device_System);
//========================================================================================
//��ȡ�豸IO������������InputConnectors������RelayOutputs����
typedef int (*GetCapabilitiesResponse_Device_IO)(void *cookies, int *InputConnectors,int *RelayOutputs);
//ע��ص��������������ص�������
void registerGetCapabilitiesResponse_Device_IO(void * ovf_env, GetCapabilitiesResponse_Device_IO _getCapabilitiesResponse_Device_IO);
//========================================================================================
//��ȡ�豸��ȫ����������������Э��֧�������0Ϊ��֧�֣�1Ϊ֧��
typedef int (*GetCapabilitiesResponse_Device_Security)(void *cookies, int *TLS1_x002e1, int *TLS1_x002e2, int *OnboardKeyGeneration, int *AccessPolicyConfig, int *X_x002e509Token, int *SAMLToken, int *KerberosToken, int *RELToken);
//ע��ص��������������ص�������
void registerGetCapabilitiesResponse_Device_Security(void * ovf_env, GetCapabilitiesResponse_Device_Security _getCapabilitiesResponse_Device_Security);
//========================================================================================
//��ȡ�豸�¼��������������¼�֧�������0Ϊ��֧�֣�1Ϊ֧��
typedef int (*GetCapabilitiesResponse_Events)(void *cookies, int *WSSubscriptionPolicySupport,int *WSPullPointSupport, int *WSPausableSubscriptionManagerInterfaceSupport);
//ע��ص��������������ص�������
void registerGetCapabilitiesResponse_Events(void * ovf_env, GetCapabilitiesResponse_Events _getCapabilitiesResponse_Events);
//========================================================================================
//��ȡ�豸ͼ����������������      (ע��ͼ����һ�����еģ��˴�ֻ��Ҫʵ��һ���պ�����ע�����)
typedef int (*GetCapabilitiesResponse_Imaging)(void *cookies);
//ע��ص��������������ص�������
void registerGetCapabilitiesResponse_Imaging(void * ovf_env, GetCapabilitiesResponse_Imaging _getCapabilitiesResponse_Imaging);
//========================================================================================
//��ȡ�豸��ý��������������RTPMulticast���ಥ����RTP_USCORETCP��RTP/TCP����RTP_USCORERTSP_USCORETCP��RTP/RTSP/TCP��0Ϊ��֧�֣�1Ϊ֧��
typedef int (*GetCapabilitiesResponse_Media)(void *cookies, int *RTPMulticast,  int *RTP_USCORETCP,int *RTP_USCORERTSP_USCORETCP);
//ע��ص��������������ص�������
void registerGetCapabilitiesResponse_Media(void * ovf_env, GetCapabilitiesResponse_Media _getCapabilitiesResponse_Media);
//========================================================================================
//����豸PTZ��������������      (ע������д˹��ܾ�ע��)
typedef int (*GetCapabilitiesResponse_PTZ)(void *cookies);
//ע��ص��������������ص�������
void registerGetCapabilitiesResponse_PTZ(void * ovf_env, GetCapabilitiesResponse_PTZ _getCapabilitiesResponse_PTZ);
//========================================================================================
//
//					��ȡ/�����������ĵ��ýӿ�
//
//========================================================================================
//��ȡ�������Ƿ�����DHCP������������DHCP��ȡ������Ϊ0����DHCP��ȡ������Ϊ1
typedef int (*GetHostnameResponse_FromDHCP)(void *cookies, int *FromDHCP);
//ע��ص��������������ص�������
void registerGetHostnameResponse_FromDHCP(void * ovf_env, GetHostnameResponse_FromDHCP _getHostnameResponse_FromDHCP);
//========================================================================================
//*****��ȡ������������������������ַ���
typedef int (*GetHostnameResponse_Name)(void *cookies, char *Name);
//ע��ص��������������ص�������
void registerGetHostnameResponse_Name(void * ovf_env, GetHostnameResponse_Name _getHostnameResponse_Name);
//========================================================================================
//*****���������������������õ��������ַ���
typedef int (*SetHostname)(void *cookies, char *Name);
//ע��ص��������������ص�������
void registerSetHostname(void * ovf_env, SetHostname _setHostname);
//========================================================================================
//
//					��ȡͼƬ����ֵ�ĵ��ýӿ�
//
//========================================================================================
//��ȡ���ⲹ��ģʽ��������0Ϊ�رգ�1Ϊ��
typedef int (*GetImagingSettingsResponse_BacklightCompensationMode)(void *cookies, int *BacklightCompensationMode);
//ע��ص��������������ص�������
void registerGetImagingSettingsResponse_BacklightCompensationMode(void * ovf_env, GetImagingSettingsResponse_BacklightCompensationMode _getImagingSettingsResponse_BacklightCompensationMode);
//========================================================================================
//*****��ȡ���ȣ��������������ֵ
typedef int (*GetImagingSettingsResponse_Brightness)(void *cookies, float *Brightness);
//ע��ص��������������ص�������
void registerGetImagingSettingsResponse_Brightness(void * ovf_env, GetImagingSettingsResponse_Brightness _getImagingSettingsResponse_Brightness);
//========================================================================================
//*****��ȡɫ�ʱ��Ͷȣ����������ɫ�ʱ��Ͷ�ֵ
typedef int (*GetImagingSettingsResponse_ColorSaturation)(void *cookies, float *ColorSaturation);
//ע��ص��������������ص�������
void registerGetImagingSettingsResponse_ColorSaturation(void * ovf_env, GetImagingSettingsResponse_ColorSaturation _getImagingSettingsResponse_ColorSaturation);
//========================================================================================
//*****��ȡ�Աȶȣ����������Աȶ�ֵ
typedef int (*GetImagingSettingsResponse_Contrast)(void *cookies, float *Contrast);
//ע��ص��������������ص�������
void registerGetImagingSettingsResponse_Contrast(void * ovf_env, GetImagingSettingsResponse_Contrast _getImagingSettingsResponse_Contrast);
//========================================================================================
//*****��ȡ��ȣ�������������ֵ
typedef int (*GetImagingSettingsResponse_Sharpness)(void *cookies, float *Sharpness);
//ע��ص��������������ص�������
void registerGetImagingSettingsResponse_Sharpness(void * ovf_env, GetImagingSettingsResponse_Sharpness _getImagingSettingsResponse_Sharpness);
//========================================================================================
//��ȡ�ع�ģʽ��������0Ϊ�Զ���1Ϊ�ֶ�
typedef int (*GetImagingSettingsResponse_ExposureMode)(void *cookies, int *ExposureMode);
//ע��ص��������������ص�������
void registerGetImagingSettingsResponse_ExposureMode(void * ovf_env, GetImagingSettingsResponse_ExposureMode _getImagingSettingsResponse_ExposureMode);
//========================================================================================
//��ȡ��С�ع�ʱ�䣬�����������С�ع�ʱ��ֵ
typedef int (*GetImagingSettingsResponse_MinExposureTime)(void *cookies, float *MinExposureTime);
//ע��ص��������������ص�������
void registerGetImagingSettingsResponse_MinExposureTime(void * ovf_env, GetImagingSettingsResponse_MinExposureTime _getImagingSettingsResponse_MinExposureTime);
//========================================================================================
//��ȡ����ع�ʱ�䣬�������������ع�ʱ��ֵ
typedef int (*GetImagingSettingsResponse_MaxExposureTime)(void *cookies, float *MaxExposureTime);
//ע��ص��������������ص�������
void registerGetImagingSettingsResponse_MaxExposureTime(void * ovf_env, GetImagingSettingsResponse_MaxExposureTime _getImagingSettingsResponse_MaxExposureTime);
//========================================================================================
//��ȡ��С�عⲹ���������������С�عⲹ��ֵ
typedef int (*GetImagingSettingsResponse_MinGain)(void *cookies, float *MinGain);
//ע��ص��������������ص�������
void registerGetImagingSettingsResponse_MinGain(void * ovf_env, GetImagingSettingsResponse_MinGain _getImagingSettingsResponse_MinGain);
//========================================================================================
//��ȡ����عⲹ�����������������عⲹ��ֵ
typedef int (*GetImagingSettingsResponse_MaxGain)(void *cookies, float *MaxGain);
//ע��ص��������������ص�������
void registerGetImagingSettingsResponse_MaxGain(void * ovf_env, GetImagingSettingsResponse_MaxGain _getImagingSettingsResponse_MaxGain);
//========================================================================================
//��ȡ������ģʽ��������0Ϊ�򿪣�1Ϊ�رգ�2Ϊ�Զ�
typedef int (*GetImagingSettingsResponse_IrCutFilterMode)(void *cookies, int *IrCutFilterMode);
//ע��ص��������������ص�������
void registerGetImagingSettingsResponse_IrCutFilterMode(void * ovf_env, GetImagingSettingsResponse_IrCutFilterMode _getImagingSettingsResponse_IrCutFilterMode);
//========================================================================================
//��ȡ��̬ģʽ��������0Ϊ�رգ�1Ϊ��
typedef int (*GetImagingSettingsResponse_WideDynamicMode)(void *cookies, int *WideDynamicMode);
//ע��ص��������������ص�������
void registerGetImagingSettingsResponse_WideDynamicMode(void * ovf_env, GetImagingSettingsResponse_WideDynamicMode _getImagingSettingsResponse_WideDynamicMode);
//========================================================================================
//��ȡ��ƽ��ģʽ��������0Ϊ�Զ���1Ϊ�ֶ�
typedef int (*GetImagingSettingsResponse_WhiteBalanceMode)(void *cookies, int *WhiteBalanceMode);
//ע��ص��������������ص�������
void registerGetImagingSettingsResponse_WhiteBalanceMode(void * ovf_env, GetImagingSettingsResponse_WhiteBalanceMode _getImagingSettingsResponse_WhiteBalanceMode);
//========================================================================================
//
//					����ͼƬ����ֵ�ĵ��ýӿ�
//
//========================================================================================
//���ñ��ⲹ��ģʽ��������0Ϊ�رգ�1Ϊ��
typedef int (*SetImagingSettings_BacklightCompensationMode)(void *cookies, int BacklightCompensationMode);
//ע��ص��������������ص�������
void registerSetImagingSettings_BacklightCompensationMode(void * ovf_env, SetImagingSettings_BacklightCompensationMode _setImagingSettings_BacklightCompensationMode);
//========================================================================================
//*****�������ȣ�����������ֵ
typedef int (*SetImagingSettings_Brightness)(void *cookies, float Brightness);
//ע��ص��������������ص�������
void registerSetImagingSettings_Brightness(void * ovf_env, SetImagingSettings_Brightness _setImagingSettings_Brightness);
//========================================================================================
//*****����ɫ�ʱ��Ͷȣ�������ɫ�ʱ��Ͷ�ֵ
typedef int (*SetImagingSettings_ColorSaturation)(void *cookies, float ColorSaturation);
//ע��ص��������������ص�������
void registerSetImagingSettings_ColorSaturation(void * ovf_env, SetImagingSettings_ColorSaturation _setImagingSettings_ColorSaturation);
//========================================================================================
//*****���öԱȶȣ��������Աȶ�ֵ
typedef int (*SetImagingSettings_Contrast)(void *cookies, float Contrast);
//ע��ص��������������ص�������
void registerSetImagingSettings_Contrast(void * ovf_env, SetImagingSettings_Contrast _setImagingSettings_Contrast);
//========================================================================================
//*****������ȣ����������ֵ
typedef int (*SetImagingSettings_Sharpness)(void *cookies, float Sharpness);
//ע��ص��������������ص�������
void registerSetImagingSettings_Sharpness(void * ovf_env, SetImagingSettings_Sharpness _setImagingSettings_Sharpness);
//========================================================================================
//�����ع�ģʽ��������0Ϊ�Զ���1Ϊ�ֶ�
typedef int (*SetImagingSettings_ExposureMode)(void *cookies, int ExposureMode);
//ע��ص��������������ص�������
void registerSetImagingSettings_ExposureMode(void * ovf_env, SetImagingSettings_ExposureMode _setImagingSettings_ExposureMode);
//========================================================================================
//������С�ع�ʱ�䣬��������С�ع�ʱ��ֵ
typedef	int (*SetImagingSettings_MinExposureTime)(void *cookies, float MinExposureTime);
//ע��ص��������������ص�������
void registerSetImagingSettings_MinExposureTime(void * ovf_env, SetImagingSettings_MinExposureTime _setImagingSettings_MinExposureTime);
//========================================================================================
//��������ع�ʱ�䣬����������ع�ʱ��ֵ
typedef	int (*SetImagingSettings_MaxExposureTime)(void *cookies, float MaxExposureTime);
//ע��ص��������������ص�������
void registerSetImagingSettings_MaxExposureTime(void * ovf_env, SetImagingSettings_MaxExposureTime _setImagingSettings_MaxExposureTime);
//========================================================================================
//������С�عⲹ������������С�عⲹ��ֵ
typedef int (*SetImagingSettings_MinGain)(void *cookies, float MinGain);
//ע��ص��������������ص�������
void registerSetImagingSettings_MinGain(void * ovf_env, SetImagingSettings_MinGain _setImagingSettings_MinGain);
//========================================================================================
//��������عⲹ��������������عⲹ��ֵ
typedef int (*SetImagingSettings_MaxGain)(void *cookies, float MaxGain);
//ע��ص��������������ص�������
void registerSetImagingSettings_MaxGain(void * ovf_env, SetImagingSettings_MaxGain _setImagingSettings_MaxGain);
//========================================================================================
//���ú�����ģʽ��������0Ϊ�򿪣�1Ϊ�رգ�2Ϊ�Զ�
typedef int (*SetImagingSettings_IrCutFilterMode)(void *cookies, int IrCutFilterMode);
//ע��ص��������������ص�������
void registerSetImagingSettings_IrCutFilterMode(void * ovf_env, SetImagingSettings_IrCutFilterMode _setImagingSettings_IrCutFilterMode);
//========================================================================================
//���ÿ�̬ģʽ��������0Ϊ�رգ�1Ϊ��
typedef int (*SetImagingSettings_WideDynamicMode)(void *cookies, int WideDynamicMode);
//ע��ص��������������ص�������
void registerSetImagingSettings_WideDynamicMode(void * ovf_env, SetImagingSettings_WideDynamicMode _setImagingSettings_WideDynamicMode);
//========================================================================================
//���ð�ƽ��ģʽ��������0Ϊ�Զ���1Ϊ�ֶ�
typedef int (*SetImagingSettings_WhiteBalanceMode)(void *cookies, int WhiteBalanceMode);
//ע��ص��������������ص�������
void registerSetImagingSettings_WhiteBalanceMode(void * ovf_env, SetImagingSettings_WhiteBalanceMode _setImagingSettings_WhiteBalanceMode);
//========================================================================================
//
//					��ȡͼƬѡ��ĵ��ýӿ�
//
//========================================================================================
//��ȡ���ⲹ��ģʽѡ�������size��ʾģʽ��ѡ�������ModeΪģʽֵ�ĺͣ�ģʽ�йرպʹ����֣��ر�ģʽֵΪ0����ģʽֵΪ1��ע��size��ModeֵӦ��ƥ�䣬���磬sizeΪ1��ModeΪ1����ʾ���ⲹ��ģʽֻ�д�ģʽѡ�sizeΪ1��ModeΪ0����ʾ���ⲹ��ģʽֻ�йر�ģʽѡ�sizeΪ2��ModeΪ1��0+1�ĺͣ�����ʾ���ⲹ��ģʽ�д򿪺͹ر�����ģʽѡ�
typedef int (*GetOptionsResponse_BacklightCompensation_Mode)(void *cookies, int *size, int *Mode);
//ע��ص��������������ص�������
void registerGetOptionsResponse_BacklightCompensation_Mode(void * ovf_env, GetOptionsResponse_BacklightCompensation_Mode _getOptionsResponse_BacklightCompensation_Mode);
//========================================================================================
//��ȡ���ⲹ���ȼ�ѡ��������ȼ�����Сֵ�����ֵ
typedef int (*GetOptionsResponse_BacklightCompensation_Level)(void *cookies, float *Min, float *Max);
//ע��ص��������������ص�������
void registerGetOptionsResponse_BacklightCompensation_Level(void * ovf_env, GetOptionsResponse_BacklightCompensation_Level _getOptionsResponse_BacklightCompensation_Level);
//========================================================================================
//*****��ȡ����ֵ��ѡ�����������ֵ����Сֵ�����ֵ
typedef int (*GetOptionsResponse_Brightness)(void *cookies, float *Min,float *Max);
//ע��ص��������������ص�������
void registerGetOptionsResponse_Brightness(void * ovf_env, GetOptionsResponse_Brightness _getOptionsResponse_Brightness);
//========================================================================================
//*****��ȡɫ�ʱ��Ͷ�ֵ��ѡ�������ɫ�ʱ��Ͷ�ֵ����Сֵ�����ֵ
typedef int (*GetOptionsResponse_ColorSaturation)(void *cookies, float *Min,float *Max);
//ע��ص��������������ص�������
void registerGetOptionsResponse_ColorSaturation(void * ovf_env, GetOptionsResponse_ColorSaturation _getOptionsResponse_ColorSaturation);
//========================================================================================
//*****��ȡ�Աȶ�ֵ��ѡ��������Աȶ�ֵ����Сֵ�����ֵ
typedef	int (*GetOptionsResponse_Contrast)(void *cookies, float *Min,float *Max);
//ע��ص��������������ص�������
void registerGetOptionsResponse_Contrast(void * ovf_env, GetOptionsResponse_Contrast _getOptionsResponse_Contrast);
//========================================================================================s
//��ȡ�ع�ģʽ��ѡ�������sizeΪģʽ���������ModeΪģʽֵ�ĺͣ��Զ�ģʽֵΪ0���ֶ�ģʽֵΪ1
typedef int (*GetOptionsResponse_Exposure_Mode)(void *cookies, int *size,int *Mode);
//ע��ص��������������ص�������
void registerGetOptionsResponse_Exposure_Mode(void * ovf_env, GetOptionsResponse_Exposure_Mode _getOptionsResponse_Exposure_Mode);
//========================================================================================
//��ȡ�ع����ȼ���ѡ�������sizeΪ���ȼ����������PriorityΪ���ȼ�ֵ�ĺͣ��������ȼ�ֵΪ0��֡�����ȼ�ֵΪ1
typedef int (*GetOptionsResponse_Exposure_Priority)(void *cookies, int *size,int *Priority);
//ע��ص��������������ص�������
void registerGetOptionsResponse_Exposure_Priority(void * ovf_env, GetOptionsResponse_Exposure_Priority _getOptionsResponse_Exposure_Priority);
//========================================================================================
//��ȡ�ع�ʱ���ѡ���������С�ع�ʱ�����Сֵ�����ֵ������ع�ʱ�����Сֵ�����ֵ
typedef int (*GetOptionsResponse_Exposure_ExposureTime)(void *cookies, int *MinExposureTime_Min,int *MinExposureTime_Max, int *MaxExposureTime_Min,int *MaxExposureTime_Max);
//ע��ص��������������ص�������
void registerGetOptionsResponse_Exposure_ExposureTime(void * ovf_env, GetOptionsResponse_Exposure_ExposureTime _getOptionsResponse_Exposure_ExposureTime);
//========================================================================================
//��ȡ�عⲹ����ѡ���������С�ع����Сֵ�����ֵ������ع����Сֵ�����ֵ
typedef int (*GetOptionsResponse_Exposure_Gain)(void *cookies, int *MinGain_Min,int *MinGain_Max,int *MaxGain_Min,int *MaxGain_Max);
//ע��ص��������������ص�������
void registerGetOptionsResponse_Exposure_Gain(void * ovf_env, GetOptionsResponse_Exposure_Gain _getOptionsResponse_Exposure_Gain);
//========================================================================================
//��ȡ������ģʽ��ѡ�������sizeΪ������ģʽ�����������IrCutFilterModesΪ������ģʽֵ�ĺͣ���ģʽֵΪ0���ر�ģʽֵΪ1���Զ�ģʽֵΪ2��ע�����磬��ģʽ+�ر�ģʽ��size = 2��IrCutFilterModes = 0+1 = 1
typedef int (*GetOptionsResponse_IrCutFilterModes)(void *cookies, int *size,int *IrCutFilterModes);
//ע��ص��������������ص�������
void registerGetOptionsResponse_IrCutFilterModes(void * ovf_env, GetOptionsResponse_IrCutFilterModes _getOptionsResponse_IrCutFilterModes);
//========================================================================================
//*****��ȡ���ֵ��ѡ����������ֵ����Сֵ�����ֵ
typedef int (*GetOptionsResponse_Sharpness)(void *cookies, float *Min,float *Max);
//ע��ص��������������ص�������
void registerGetOptionsResponse_Sharpness(void * ovf_env, GetOptionsResponse_Sharpness  _getOptionsResponse_Sharpness);
//========================================================================================
//��ȡ��̬��Χģʽ��ѡ�������sizeΪģʽ�����������ModeΪģʽֵ�ĺͣ��ر�ģʽֵΪ0����ģʽֵΪ1
typedef	int (*GetOptionsResponse_WideDynamicRange_Mode)(void *cookies, int *size,int *Mode);
//ע��ص��������������ص�������
void registerGetOptionsResponse_WideDynamicRange_Mode(void * ovf_env, GetOptionsResponse_WideDynamicRange_Mode _getOptionsResponse_WideDynamicRange_Mode);
//========================================================================================
//��ȡ��̬��Χ�ȼ���ѡ��������ȼ�����Сֵ�����ֵ
typedef int (*GetOptionsResponse_WideDynamicRange_Level)(void *cookies, float *Min,float *Max);
//ע��ص��������������ص�������
void registerGetOptionsResponse_WideDynamicRange_Level(void * ovf_env, GetOptionsResponse_WideDynamicRange_Level _getOptionsResponse_WideDynamicRange_Level);
//========================================================================================
//��ȡ��ƽ��ģʽ��ѡ�������sizeΪģʽ�����������ModeΪģʽֵ�ĺͣ��Զ�ģʽֵΪ0���ֶ�ģʽֵΪ1
typedef int (*GetOptionsResponse_WhiteBalance_Mode)(void *cookies, int *size,int *Mode);
//ע��ص��������������ص�������
void registerGetOptionsResponse_WhiteBalance_Mode(void * ovf_env, GetOptionsResponse_WhiteBalance_Mode _getOptionsResponse_WhiteBalance_Mode);
//========================================================================================
//��ȡ��ƽ��Yr������ѡ�������Yr������Сֵ�����ֵ
typedef int (*GetOptionsResponse_WhiteBalance_YrGain)(void *cookies, float *Min,float *Max);
//ע��ص��������������ص�������
void registerGetOptionsResponse_WhiteBalance_YrGain(void * ovf_env, GetOptionsResponse_WhiteBalance_YrGain _getOptionsResponse_WhiteBalance_YrGain);
//========================================================================================
//��ȡ��ƽ��Yb������ѡ�������Yb������Сֵ�����ֵ
typedef int (*GetOptionsResponse_WhiteBalance_YbGain)(void *cookies, float *Min,float *Max);
//ע��ص��������������ص�������
void registerGetOptionsResponse_WhiteBalance_YbGain(void * ovf_env, GetOptionsResponse_WhiteBalance_YbGain _getOptionsResponse_WhiteBalance_YbGain);
//========================================================================================
//
//					��ȡ��Ƶ����Uri
//
//========================================================================================
//*****��ȡ��Ƶ��Uri��������StreamToken,���ַ�����ʶ��Ƶ������MainStream����ʶ����������SubStream����ʶ������; Uri���ڱ�����Ƶuri
typedef	int (*GetStreamUriResponse_Uri)(void *cookies, char *StreamToken, char * Uri);
//ע��ص��������������ص�������
void registerGetStreamUriResponse_Uri(void * ovf_env, GetStreamUriResponse_Uri  _getStreamUriResponse_Uri);
//========================================================================================
//
//					��ȡ��Ƶ��ͼ��Uri	
//
//========================================================================================
//*****��ȡ��Ƶ��ͼUri��������StreamToken,���ַ�����ʶ��Ƶ������MainStream����ʶ����������SubStream����ʶ������;Uri���ڱ�����Ƶ��ͼuri
typedef int (*GetSnapshotUriResponse_Uri)(void *cookies, char *StreamToken, char * Uri);
//ע��ص��������������ص�������
void registerGetSnapshotUriResponse_Uri(void * ovf_env, GetSnapshotUriResponse_Uri _getSnapshotUriResponse_Uri);
//========================================================================================
//
//					��ȡ���������õĵ��ýӿ�
//
//========================================================================================
//��ȡ�������ı������ͣ�������tokenָ��Ҫ���õı���������MainStream��Ϊ�������������SubStream��Ϊ�����������Encoding��0ΪJPEG��1ΪMPEG4��2ΪH264
typedef int (*GetVideoEncoderConfigurationResponse_Encoding)(void *cookies, const char *token, int *Encoding);
//ע��ص��������������ص�������
void registerGetVideoEncoderConfigurationResponse_Encoding(void * ovf_env, GetVideoEncoderConfigurationResponse_Encoding _getVideoEncoderConfigurationResponse_Encoding);
//========================================================================================
//*****��ȡ�������ı���ֱ��ʣ�������tokenָ��Ҫ���õı���������MainStream��Ϊ�������������SubStream��Ϊ������������ֱ��ʵĿ�͸�
typedef int (*GetVideoEncoderConfigurationResponse_Resolution)(void *cookies, const char *token, int *Width, int *Height);
//ע��ص��������������ص�������
void registerGetVideoEncoderConfigurationResponse_Resolution(void * ovf_env, GetVideoEncoderConfigurationResponse_Resolution _getVideoEncoderConfigurationResponse_Resolution);
//========================================================================================
//��ȡ�������ı���������������tokenָ��Ҫ���õı���������MainStream��Ϊ�������������SubStream��Ϊ�������������������
typedef int (*GetVideoEncoderConfigurationResponse_Quality)(void *cookies, const char *token, float *Quality);
//ע��ص��������������ص�������
void registerGetVideoEncoderConfigurationResponse_Quality(void * ovf_env, GetVideoEncoderConfigurationResponse_Quality _getVideoEncoderConfigurationResponse_Quality);
//========================================================================================
//*****��ȡ�����������ʿ��ƣ�������tokenָ��Ҫ���õı���������MainStream��Ϊ�������������SubStream��Ϊ�����������֡�ʡ���������������
typedef int (*GetVideoEncoderConfigurationResponse_RateControl)(void *cookies, const char *token, int *FrameRateLimit, int *EncodingInterval, int *BitrateLimit);
//ע��ص��������������ص�������
void registerGetVideoEncoderConfigurationResponse_RateControl(void * ovf_env, GetVideoEncoderConfigurationResponse_RateControl _getVideoEncoderConfigurationResponse_RateControl);
//========================================================================================
//��ȡ��������H264���ã�������tokenָ��Ҫ���õı���������MainStream��Ϊ�������������SubStream��Ϊ�����������Govlength��H264������أ�Ŀǰ��������壩��
//							  H264Profile:0ΪBaseline��1ΪMain��2ΪExtended��3ΪHigh
typedef int (*GetVideoEncoderConfigurationResponse_H264)(void *cookies, const char *token, int *GovLength, int *H264Profile);
//ע��ص��������������ص�������
void registerGetVideoEncoderConfigurationResponse_H264(void * ovf_env, GetVideoEncoderConfigurationResponse_H264 _getVideoEncoderConfigurationResponse_H264);
//========================================================================================
//��ȡ�������Ķಥ���ԣ�������tokenָ��Ҫ���õı���������MainStream��Ϊ�������������SubStream��Ϊ�����������
//                            Typeָ���ಥ�ĵ�ַ���ͣ�0ΪIPv4��1ΪIPv6��IPv4Address��IPv6Address����Typeֵֻ�õ�һ����PortΪ�˿ںš�TTLΪ����ʱ�䣬
//                            AutoStartΪ�Զ���ʼ����,0Ϊ�رգ�1Ϊ��
typedef int (*GetVideoEncoderConfigurationResponse_Multicast)(void *cookies, const char *token, int *Type, char *IPv4Address, char *IPv6Address, int *Port, int *TTL, int *AutoStart);
//ע��ص��������������ص�������
void registerGetVideoEncoderConfigurationResponse_Multicast(void * ovf_env, GetVideoEncoderConfigurationResponse_Multicast _getVideoEncoderConfigurationResponse_Multicast);
//========================================================================================
//��ȡ�������ĻỰ��ʱ��������tokenָ��Ҫ���õı���������MainStream��Ϊ�������������SubStream��Ϊ�������������ʱʱ��ֵ
typedef int (*GetVideoEncoderConfigurationResponse_SessionTimeout)(void *cookies, const char *token, long long *SessionTimeout);
//ע��ص��������������ص�������
void registerGetVideoEncoderConfigurationResponse_SessionTimeout(void * ovf_env, GetVideoEncoderConfigurationResponse_SessionTimeout _getVideoEncoderConfigurationResponse_SessionTimeout);
//========================================================================================
//
//					���ñ��������õĵ��ýӿ�
//
//========================================================================================
//���ñ������ı������ͣ�������tokenָ��Ҫ���õı���������MainStream��Ϊ�������������SubStream��Ϊ�����������Encoding��0ΪJPEG��1ΪMPEG4��2ΪH264
typedef int (*SetVideoEncoderConfiguration_Encoding)(void *cookies, const char *token, int Encoding);
//ע��ص��������������ص�������
void registerSetVideoEncoderConfiguration_Encoding(void * ovf_env, SetVideoEncoderConfiguration_Encoding _setVideoEncoderConfiguration_Encoding);
//========================================================================================
//*****���ñ������ı���ֱ��ʣ�������tokenָ��Ҫ���õı���������MainStream��Ϊ�������������SubStream��Ϊ������������ֱ��ʵĿ�͸�
typedef int (*SetVideoEncoderConfiguration_Resolution)(void *cookies, const char *token, int Width, int Height);
//ע��ص��������������ص�������
void registerSetVideoEncoderConfiguration_Resolution(void * ovf_env, SetVideoEncoderConfiguration_Resolution _setVideoEncoderConfiguration_Resolution);
//========================================================================================
//���ñ������ı���������������tokenָ��Ҫ���õı���������MainStream��Ϊ�������������SubStream��Ϊ�������������������
typedef int (*SetVideoEncoderConfiguration_Quality)(void *cookies, const char *token, float Quality);
//ע��ص��������������ص�������
void registerSetVideoEncoderConfiguration_Quality(void * ovf_env, SetVideoEncoderConfiguration_Quality _setVideoEncoderConfiguration_Quality);
//========================================================================================
//*****���ñ����������ʿ��ƣ�������tokenָ��Ҫ���õı���������MainStream��Ϊ�������������SubStream��Ϊ�����������֡�ʡ���������������
typedef int (*SetVideoEncoderConfiguration_RateControl)(void *cookies, const char *token, int FrameRateLimit, int EncodingInterval, int BitrateLimit);
//ע��ص��������������ص�������
void registerSetVideoEncoderConfiguration_RateControl(void * ovf_env, SetVideoEncoderConfiguration_RateControl _setVideoEncoderConfiguration_RateControl);
//========================================================================================
//���ñ�������H264���ã�������tokenָ��Ҫ���õı���������MainStream��Ϊ�������������SubStream��Ϊ�����������Govlength��H264������أ�Ŀǰ��������壩��
//							  H264Profile:0ΪBaseline��1ΪMain��2ΪExtended��3ΪHigh
typedef int (*SetVideoEncoderConfiguration_H264)(void *cookies, const char *token, int GovLength, int H264Profile);
//ע��ص��������������ص�������
void registerSetVideoEncoderConfiguration_H264(void * ovf_env, SetVideoEncoderConfiguration_H264 _setVideoEncoderConfiguration_H264);
//========================================================================================
//���ñ������Ķಥ���ԣ�������tokenָ��Ҫ���õı���������MainStream��Ϊ�������������SubStream��Ϊ�����������
//                            Typeָ���ಥ�ĵ�ַ���ͣ�0ΪIPv4��1ΪIPv6��IPv4Address��IPv6Address����Typeֵֻ�õ�һ����PortΪ�˿ںš�TTLΪ����ʱ�䣬
//                            AutoStartΪ�Զ���ʼ����,0Ϊ�رգ�1Ϊ��
typedef int (*SetVideoEncoderConfiguration_Multicast)(void *cookies, const char *token, int Type, char *IPv4Address, char *IPv6Address, int Port, int TTL, int AutoStart);
//ע��ص��������������ص�������
void registerSetVideoEncoderConfiguration_Multicast(void * ovf_env, SetVideoEncoderConfiguration_Multicast _setVideoEncoderConfiguration_Multicast);
//========================================================================================
//���ñ������ĻỰ��ʱ��������tokenָ��Ҫ���õı���������MainStream��Ϊ�������������SubStream��Ϊ�������������ʱʱ��ֵ
typedef int (*SetVideoEncoderConfiguration_SessionTimeout)(void *cookies, const char *token, long long SessionTimeout);
//ע��ص��������������ص�������
void registerSetVideoEncoderConfiguration_SessionTimeout(void * ovf_env, SetVideoEncoderConfiguration_SessionTimeout _setVideoEncoderConfiguration_SessionTimeout);
//========================================================================================
//
//					��ȡOSD��Ϣ�ĵ��ýӿ�
//
//========================================================================================
//*****��ȡOSD����ʾλ�ã�������type���OSD������("DateTime"��ʶʱ�䣬"TextString"��ʶ�ı��ַ���)��OSD��ʾ����x��y
typedef int (*GetOSD_Position)(void *cookies, char *type, float *x, float *y);
//ע��ص��������������ص�������
void registerGetOSD_Position(void * ovf_env, GetOSD_Position _getOSD_Position);
//*****��ȡOSD�Ĵ���״̬��������type���OSD������("DateTime"��ʶʱ�䣬"TextString"��ʶ�ı��ַ���),OSD״̬Ϊ������flagΪ0��OSD״̬Ϊ����flagΪ1
//					      ����ֵ:ʧ�ܷ���ֵΪ-1���ɹ�����ֵΪ0
typedef int (*GetOSD_Status)(void *cookies, char *type, int *flag);
//ע��ص��������������ص�������
void registerGetOSD_Status(void * ovf_env, GetOSD_Status _getOSD_Status);
//*****��ȡOSD���ı��ַ�����������OSD���ı��ַ���
typedef int (*GetOSD_Text)(void *cookies, char * text);
//ע��ص��������������ص�������
void registerGetOSD_Text(void * ovf_env, GetOSD_Text _getOSD_Text);
//========================================================================================
//
//					����OSD��Ϣ�ĵ��ýӿ�
//
//========================================================================================
//*****����IPC�豸OSDͼƬ��URL��������type���OSD������("TextString"��ʶ�ı��ַ�����URL��"ShowWords"��ʶOSD����ʾ������)��UrlͼƬURL������OSD�е�����
typedef int (*SetOSD_URL)(void *cookies, char *type, char *Url);
//ע��ص��������������ص�������
void registerSetOSD_URL(void * ovf_env, SetOSD_URL _setOSD_URL);
//*****����OSD����ʾλ�ã�������type���OSD������("DateTime"��ʶʱ�䣬"TextString"��ʶ�ı��ַ���)��OSD��ʾ����x��y
typedef int (*SetOSD_Position)(void *cookies, char *type, float x, float y);
//ע��ص��������������ص�������
void registerSetOSD_Position(void * ovf_env, SetOSD_Position _setOSD_Position);
//========================================================================================
//
//					����OSD�ĵ��ýӿ�
//
//========================================================================================
//*****������Ӧ��OSD��������type���OSD������("DateTime"��ʶʱ�䣬"TextString"��ʶ�ı��ַ���)
typedef int (*CreateTheOSD)(void *cookies, char *type);
//ע��ص��������������ص�������
void registerCreateTheOSD(void * ovf_env, CreateTheOSD _createTheOSD);
//========================================================================================
//
//					ɾ��OSD�ĵ��ýӿ�
//
//========================================================================================
//*****ɾ����Ӧ��OSD��������type���OSD������("DateTime"��ʶʱ�䣬"TextString"��ʶ�ı��ַ���)
typedef int (*DeleteTheOSD)(void *cookies, char *type);
//ע��ص��������������ص�������
void registerDeleteTheOSD(void * ovf_env, DeleteTheOSD _deleteTheOSD);
//========================================================================================
//
//					�ƶ����ĵ��ýӿ�
//
//========================================================================================
//*****���ƶ���⹦�ܣ���������
typedef int (*OpenMotionDetector)(void *cookies);
//ע��ص��������������ص�������
void registerOpenMotionDetector(void * ovf_env, OpenMotionDetector _openMotionDetector);
//*****�ر��ƶ���⹦�ܣ���������
typedef int (*CloseMotionDetector)(void *cookies);
//ע��ص��������������ص�������
void registerCloseMotionDetector(void * ovf_env, CloseMotionDetector _closeMotionDetector);
//*****��⵽�ƶ��¼���������flag��ʶ������Ϣ�Ĵ���״̬��0��ʾû�б�����Ϣ��1��ʾ�б�����Ϣ
//					   ����ֵ��ʧ�ܷ���ֵΪ-1,�ɹ�����ֵΪ0
typedef int (*MotionAlarm)(void *cookies, int * flag);
//ע��ص��������������ص�������
void registerMotionAlarm(void * ovf_env, MotionAlarm _motionAlarm);
//*****��⹦�ܵĿ���״̬��������type��ʶ������ͣ�flag��ʶ״̬���ر�ʱΪ0������ʱΪ1
//					       ����ֵ��ʧ�ܷ���ֵΪ-1���ɹ�����ֵΪ0
typedef int (*DetectorOpenOrNot)(void *cookies, char * type, int *flag);
//ע��ص��������������ص�������
void registerDetectorOpenOrNot(void * ovf_env, DetectorOpenOrNot _detectorOpenOrNot);
//*****�趨������򣬲�����sizeΪbuff�ĸ�ʽ���ȣ�buffΪ�����趨����
typedef int (*SetDetectionArea)(void *cookies, int size, unsigned char * buff);
//ע��ص��������������ص�������
void registerSetDetectionArea(void * ovf_env, SetDetectionArea _setDetectionArea);
//========================================================================================
//
//					NTP�ͻ��˵ĵ��ýӿ�
//
//========================================================================================
//*****��NTP�ͻ��ˣ���������
typedef int (*OpenNTP)(void *cookies);
//ע��ص��������������ص�������
void registerOpenNTP(void * ovf_env, OpenNTP _openNTP);
//*****�ر�NTP�ͻ��ˣ���������
typedef int (*CloseNTP)(void *cookies);
//ע��ص��������������ص�������
void registerCloseNTP(void * ovf_env, CloseNTP _closeNTP);
//*****NTP���ܵĿ���״̬��������flag��ʶNTP����״̬��0��ʾ�ر�״̬��1��ʾ����״̬
//                        ����ֵ��ʧ�ܷ���ֵΪ-1���ɹ�����ֵΪ0
typedef int (*TheNTPOpenOrNot)(void *cookies, int *flag);
//ע��ص��������������ص�������
void registerTheNTPOpenOrNot(void * ovf_env, TheNTPOpenOrNot _theNTPOpenOrNot);



#ifdef __cplusplus
}
#endif

#endif /* onvif_ipc.h */