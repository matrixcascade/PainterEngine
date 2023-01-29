#include "ntddk.h"
#include "ntddkbd.h"
#include "windef.h"


#define KBD_DRIVER_NAME  L"\\Driver\\Kbdclass"
#define PS2_DRIVER_NAME  L"\\Driver\\i8042prt"
#define USB_DRIVER_NAME  L"\\Driver\\Kbdhid"

#define SCAN_CODE_STACK_SIZE 1024
#define SEARCH_LOG_SIZE 1024
typedef struct PX_VKdata_Intialize
{
	ULONG ret;
	char content[SEARCH_LOG_SIZE];
}PX_VKdata_Intialize;

typedef struct PX_VKdata_AddEvent
{
	ULONG scan_code;
	ULONG Iskeydown;
	ULONG ret;
}PX_VKdata_AddEvent;

typedef struct PX_VKdata_GetEvent
{
	USHORT scan_code_stack[SCAN_CODE_STACK_SIZE];
	ULONG  scan_code_size;
}PX_VKdata_GetEvent;

typedef enum
{
	PX_DRIVER_IOCTL_OPCODE_INIT = 0x801,
	PX_DRIVER_IOCTL_OPCODE_ADDKEYEVENT = 0x802,
	PX_DRIVER_IOCTL_OPCODE_GETKEYEVENT = 0x803,
}PX_DRIVER_IOCTL_OPCODE;

#define PX_DRIVER_IOCTL_CODE_INIT CTL_CODE(FILE_DEVICE_UNKNOWN, PX_DRIVER_IOCTL_OPCODE_INIT, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define PX_DRIVER_IOCTL_CODE_ADDEVENT CTL_CODE(FILE_DEVICE_UNKNOWN, PX_DRIVER_IOCTL_OPCODE_ADDKEYEVENT, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define PX_DRIVER_IOCTL_CODE_GETEVENT CTL_CODE(FILE_DEVICE_UNKNOWN, PX_DRIVER_IOCTL_OPCODE_GETKEYEVENT, METHOD_BUFFERED, FILE_ANY_ACCESS)

NTSTATUS ObReferenceObjectByName(PUNICODE_STRING ObjectName,
    ULONG Attributes,
    PACCESS_STATE AccessState,
    ACCESS_MASK DesiredAccess,
    POBJECT_TYPE ObjectType,
    KPROCESSOR_MODE AccessMode,
    PVOID ParseContext,
    PVOID* Object
);

extern POBJECT_TYPE* IoDriverObjectType;


typedef VOID(_stdcall* KeyboardClassServiceCallback)
(IN PDEVICE_OBJECT DeviceObject,
    IN PKEYBOARD_INPUT_DATA InputDataStart,
    IN PKEYBOARD_INPUT_DATA InputDataEnd,
    IN OUT PULONG InputDataConsumed);



typedef struct _KBD_CALLBACK
{
    PDEVICE_OBJECT classDeviceObject;
    KeyboardClassServiceCallback serviceCallBack;
	PVOID* pserviceCallBack_hookaddr;
}KBD_CALLBACK;

KBD_CALLBACK g_KbdCallBack = {0};

#define  DELAY_ONE_MICROSECOND  (-10)
#define  DELAY_ONE_MILLISECOND (DELAY_ONE_MICROSECOND*1000)
#define  DELAY_ONE_SECOND (DELAY_ONE_MILLISECOND*1000)

char searchlog[SEARCH_LOG_SIZE] = {0};
unsigned short scan_code_stack[SCAN_CODE_STACK_SIZE];
unsigned int  scan_code_ptr = 0;
unsigned int  scan_code_size = 0;

VOID _stdcall PX_PainterEngineVKCallback
(IN PDEVICE_OBJECT DeviceObject,
	IN PKEYBOARD_INPUT_DATA InputDataStart,
	IN PKEYBOARD_INPUT_DATA InputDataEnd,
	IN OUT PULONG InputDataConsumed)
{

	PKEYBOARD_INPUT_DATA pdata;
	for (pdata = InputDataStart;pdata< InputDataEnd;pdata++)
	{
		scan_code_stack[scan_code_ptr] = pdata->MakeCode;

		if (pdata->Flags == KEY_MAKE)
			scan_code_stack[scan_code_ptr] |= 0x8000;
		else
			scan_code_stack[scan_code_ptr] &= 0x7fff;

		scan_code_ptr++;
		scan_code_ptr %= SCAN_CODE_STACK_SIZE;
		if (scan_code_size< SCAN_CODE_STACK_SIZE)
			scan_code_size++;
	}
	g_KbdCallBack.serviceCallBack(DeviceObject, InputDataStart, InputDataEnd, InputDataConsumed);
}

NTSTATUS PX_HookKbdDevice()
{
    NTSTATUS status = STATUS_UNSUCCESSFUL;
    UNICODE_STRING uniNtNameString;
    PDEVICE_OBJECT pCurrentKbdDeviceObject = NULL;
    PDRIVER_OBJECT KbdDriverObject = NULL;
    PDRIVER_OBJECT KbdhidDriverObject = NULL;
    PDRIVER_OBJECT Kbd8042DriverObject = NULL;
    PDRIVER_OBJECT TopKdbDriverObject = NULL;

    PVOID KbdDriverStart = NULL;
    ULONG KbdDriverSize = 0;
    PBYTE pCurrentDeviceExt = NULL;
    ULONG i = 0;
    PVOID pTemp;
    PDEVICE_OBJECT pKbdDevice;
    searchlog[0] = 0;
    RtlInitUnicodeString(&uniNtNameString, PS2_DRIVER_NAME);
    status = ObReferenceObjectByName(
        &uniNtNameString,
        OBJ_CASE_INSENSITIVE,
        NULL,
        0,
        *IoDriverObjectType,
        KernelMode,
        NULL,
        (PVOID*)&Kbd8042DriverObject
    );
    if (!NT_SUCCESS(status))
    {
        strcat(searchlog,"Couldn't get the PS/2 driver Object\n");
    }
    else
    {
        ObDereferenceObject(Kbd8042DriverObject);
        strcat(searchlog, "Got the PS/2 driver Object\n");
    }


    RtlInitUnicodeString(&uniNtNameString, USB_DRIVER_NAME);
    status = ObReferenceObjectByName(
        &uniNtNameString,
        OBJ_CASE_INSENSITIVE,
        NULL,
        0,
        *IoDriverObjectType,
        KernelMode,
        NULL,
        (PVOID*)&KbdhidDriverObject
    );

    if (NT_SUCCESS(status))
    {
        ObDereferenceObject(KbdhidDriverObject);
    }

    if (!Kbd8042DriverObject && !KbdhidDriverObject)
    {
        strcat(searchlog, "Not found keyboard!\n");
        return STATUS_UNSUCCESSFUL;
    }

    TopKdbDriverObject = Kbd8042DriverObject ? Kbd8042DriverObject : KbdhidDriverObject;

    RtlInitUnicodeString(&uniNtNameString, KBD_DRIVER_NAME);
    status = ObReferenceObjectByName(
        &uniNtNameString,
        OBJ_CASE_INSENSITIVE,
        NULL,
        0,
        *IoDriverObjectType,
        KernelMode,
        NULL,
        (PVOID*)&KbdDriverObject
    );

    if (!NT_SUCCESS(status))
    {
        strcat(searchlog, "Couldn't get the Kbdclass Object\n");
        return STATUS_UNSUCCESSFUL;
    }
    else
    {
        ObDereferenceObject(KbdDriverObject);
    }

    KbdDriverStart = KbdDriverObject->DriverStart;
    KbdDriverSize = KbdDriverObject->DriverSize;
    
   
    g_KbdCallBack.serviceCallBack = 0;

	pKbdDevice = KbdDriverObject->DeviceObject;
	

	while (pKbdDevice)
	{
		pCurrentKbdDeviceObject = TopKdbDriverObject->DeviceObject;
		while (pCurrentKbdDeviceObject)
		{

			PDEVICE_OBJECT pCurrentAttachedDevice = pCurrentKbdDeviceObject->AttachedDevice;
			while (pCurrentAttachedDevice)
			{

				if (pKbdDevice == pCurrentAttachedDevice)
				{
					pCurrentDeviceExt = (PBYTE)pCurrentKbdDeviceObject->DeviceExtension;

					for (i = 0; i < 4096; i++, pCurrentDeviceExt += sizeof(PBYTE))
					{
						if (!MmIsAddressValid(pCurrentDeviceExt))
						{
							break;
						}

						pTemp = *(PVOID*)pCurrentDeviceExt;

						if (pTemp == pKbdDevice)
						{
							g_KbdCallBack.classDeviceObject = (PDEVICE_OBJECT)pTemp;
							continue;
						}
						else if ((pTemp > KbdDriverStart) && ((PBYTE)pTemp < (PBYTE)KbdDriverStart + KbdDriverSize) && MmIsAddressValid(pTemp))
						{
							g_KbdCallBack.serviceCallBack = (KeyboardClassServiceCallback)pTemp;
							status = STATUS_SUCCESS;
							g_KbdCallBack.pserviceCallBack_hookaddr = (PVOID*)pCurrentDeviceExt;
							*g_KbdCallBack.pserviceCallBack_hookaddr = (PVOID)PX_PainterEngineVKCallback;
							return status;
						}

					}
				}

				//next attach
				pCurrentAttachedDevice = pCurrentAttachedDevice->AttachedDevice;
			}

			pCurrentKbdDeviceObject = pCurrentKbdDeviceObject->NextDevice;
		}

		pKbdDevice = pKbdDevice->NextDevice;
	}

  
    strcat(searchlog, "kbcalss not found\n");
    status = STATUS_UNSUCCESSFUL;
    return status;
}



NTSTATUS PX_DriverDispatch(IN PDEVICE_OBJECT pDeviceObject, IN PIRP pIrp)
{
	PIO_STACK_LOCATION irpStack;
	ULONG InputBufferLength, OutputBufferLength;
	ULONG opcode;
	void* piobuffer;
	NTSTATUS retStatus;
	

	pDeviceObject = 0;
	pIrp->IoStatus.Status = STATUS_SUCCESS;
	pIrp->IoStatus.Information = 0;
	irpStack = IoGetCurrentIrpStackLocation(pIrp);

	piobuffer = pIrp->AssociatedIrp.SystemBuffer;
	InputBufferLength = irpStack->Parameters.DeviceIoControl.InputBufferLength;
	OutputBufferLength = irpStack->Parameters.DeviceIoControl.OutputBufferLength;
	

	switch (irpStack->MajorFunction)
	{
	case IRP_MJ_CREATE:
	case IRP_MJ_CLOSE:
		break;

	case IRP_MJ_DEVICE_CONTROL:
	{
		opcode = irpStack->Parameters.DeviceIoControl.IoControlCode;
		switch (opcode)
		{
		case PX_DRIVER_IOCTL_CODE_INIT:
		{
			PX_VKdata_Intialize* piodata;
			if (InputBufferLength == sizeof(PX_VKdata_Intialize)&& OutputBufferLength== sizeof(PX_VKdata_Intialize))
			{
				piodata=(PX_VKdata_Intialize*)piobuffer;
                pIrp->IoStatus.Information = sizeof(PX_VKdata_Intialize);
				
				if(!g_KbdCallBack.serviceCallBack)
					PX_HookKbdDevice();

                if (g_KbdCallBack.serviceCallBack)
                {
                    piodata->ret = 1;
                }
                else
                {
					piodata->ret = 0;
                }
				memcpy(piodata->content, searchlog,sizeof(searchlog));
				pIrp->IoStatus.Status = STATUS_SUCCESS;
			}
			else
			{
				pIrp->IoStatus.Status = STATUS_INVALID_PARAMETER;
			}
		}
		break;
		case PX_DRIVER_IOCTL_CODE_ADDEVENT:
		{
			if (InputBufferLength == sizeof(PX_VKdata_AddEvent) && OutputBufferLength == sizeof(PX_VKdata_AddEvent))
			{
				PX_VKdata_AddEvent *piodata = (PX_VKdata_AddEvent*)piobuffer;
				
                pIrp->IoStatus.Information = sizeof(PX_VKdata_AddEvent);
				if (g_KbdCallBack.serviceCallBack)
				{
					ULONG InputDataConsumed = 0;
					KEYBOARD_INPUT_DATA KbdInData[2] = { 0 };
					KbdInData[0].MakeCode = (USHORT)piodata->scan_code;
					KbdInData[0].Flags = piodata->Iskeydown?KEY_MAKE:KEY_BREAK;
					g_KbdCallBack.serviceCallBack(g_KbdCallBack.classDeviceObject, &KbdInData[0], &KbdInData[1], &InputDataConsumed);
					piodata->ret = 1;
				}
                else
                {
					piodata->ret = 0;
                }
				pIrp->IoStatus.Status = STATUS_SUCCESS;
			}
			else
			{
				pIrp->IoStatus.Status = STATUS_INVALID_PARAMETER;
			}
		}
		break;
		case PX_DRIVER_IOCTL_CODE_GETEVENT:
		{
			if (InputBufferLength == sizeof(PX_VKdata_GetEvent) && OutputBufferLength == sizeof(PX_VKdata_GetEvent))
			{
				ULONG i;
				PX_VKdata_GetEvent *piodata = (PX_VKdata_GetEvent*)piobuffer;
				pIrp->IoStatus.Information = sizeof(PX_VKdata_GetEvent);
				piodata->scan_code_size = scan_code_size;
				for (i=0;i<scan_code_size;i++)
				{
					piodata->scan_code_stack[i] = scan_code_stack[(scan_code_ptr+ SCAN_CODE_STACK_SIZE - scan_code_size + i) % SCAN_CODE_STACK_SIZE];
				}
				scan_code_size = 0;
				scan_code_ptr = 0;
				pIrp->IoStatus.Status = STATUS_SUCCESS;
			}
			else
			{
				pIrp->IoStatus.Status = STATUS_INVALID_PARAMETER;
			}
		}
		break;
		default:
			pIrp->IoStatus.Status = STATUS_INVALID_PARAMETER;
			break;
		}
	}
	break;

	default:
		pIrp->IoStatus.Status = STATUS_INVALID_PARAMETER;
		break;
	}

	retStatus = pIrp->IoStatus.Status;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return retStatus;
}
void PX_DriverUnload(IN PDRIVER_OBJECT pDriverObject)
{
	UNICODE_STRING DeviceLinkUnicodeString;
	NTSTATUS ntStatus;
	RtlInitUnicodeString(&DeviceLinkUnicodeString, L"\\DosDevices\\PainterEngineVK");
	ntStatus = IoDeleteSymbolicLink(&DeviceLinkUnicodeString);
	if (NT_SUCCESS(ntStatus))
	{
		IoDeleteDevice(pDriverObject->DeviceObject);
	}
	*g_KbdCallBack.pserviceCallBack_hookaddr = (PVOID)g_KbdCallBack.serviceCallBack;
	memset(&g_KbdCallBack,0, sizeof(g_KbdCallBack));
}
NTSTATUS DriverEntry(IN PDRIVER_OBJECT pDriverObject, IN PUNICODE_STRING RegistryPath)
{
	UNICODE_STRING  DeviceNameUnicodeString;
	UNICODE_STRING  DeviceLinkUnicodeString;
	NTSTATUS        ntStatus;
	PDEVICE_OBJECT  DeviceObject = 0;
	RegistryPath = 0;

	RtlInitUnicodeString(&DeviceNameUnicodeString, L"\\Device\\PainterEngineVK");

	ntStatus = IoCreateDevice(pDriverObject, 0, &DeviceNameUnicodeString, FILE_DEVICE_UNKNOWN, 0, 0, &DeviceObject);


	if (NT_SUCCESS(ntStatus))
	{
		pDriverObject->MajorFunction[IRP_MJ_CREATE] = PX_DriverDispatch;
		pDriverObject->MajorFunction[IRP_MJ_CLOSE] = PX_DriverDispatch;
		pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = PX_DriverDispatch;
		pDriverObject->DriverUnload = PX_DriverUnload;

		RtlInitUnicodeString(&DeviceLinkUnicodeString, L"\\DosDevices\\PainterEngineVK");

		ntStatus = IoCreateSymbolicLink(&DeviceLinkUnicodeString, &DeviceNameUnicodeString);

		if (!NT_SUCCESS(ntStatus))
		{
			KdPrint(("PainterEngineVK Failed\n"));
			IoDeleteDevice(DeviceObject);
		}
		KdPrint(("PainterEngineVK Entry\n"));
	}
	return ntStatus;
}

