#include "DispatchProcedures.h"

#include "RLoggerCommon.h"
#include "Globals.h"

extern Globals g_Globals;

NTSTATUS CompleteIrp(PIRP Irp, NTSTATUS status, ULONG_PTR info);

NTSTATUS RLoggerCreateClose(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp) {
    UNREFERENCED_PARAMETER(DeviceObject);

    KdPrint(("RLoggerCreateClose called\n"));

    return CompleteIrp(Irp, STATUS_SUCCESS, 0);
}

NTSTATUS RLoggerDeviceControl(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp) {
    UNREFERENCED_PARAMETER(DeviceObject);

    KdPrint(("RLoggerDeviceControl called\n"));

    PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
    NTSTATUS status = STATUS_SUCCESS;

    switch (stack->Parameters.DeviceIoControl.IoControlCode) {
    case IOCTL_RLOGGER_SET_TARGET_PID:
        KdPrint(("IOCTL_RLOGGER_SET_TARGET_PID called\n"));

        if (stack->Parameters.DeviceIoControl.InputBufferLength < sizeof(ULONG)) {
            status = STATUS_BUFFER_TOO_SMALL;
            break;
        }

        g_Globals.targetPID = *(ULONG*)stack->Parameters.DeviceIoControl.Type3InputBuffer;
        g_Globals.isLoggingActive = TRUE;
        KdPrint(("Target PID changed to %u\n", g_Globals.targetPID));
        break;

    case IOCTL_RLOGGER_STOP_LOGGING:
        KdPrint(("IOCTL_RLOGGER_STOP_LOGGING called\n"));

        g_Globals.isLoggingActive = FALSE;
        KdPrint(("Logging stopped\n"));
        break;

    default:
        KdPrint(("Invalid device request\n"));
        status = STATUS_INVALID_DEVICE_REQUEST;
        break;
    }

    return CompleteIrp(Irp, status, 0);
}

NTSTATUS CompleteIrp(PIRP Irp, NTSTATUS status, ULONG_PTR info) {
    Irp->IoStatus.Status = status;
    Irp->IoStatus.Information = info;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    KdPrint(("IRP completed with status 0x%X, information %lu\n", status, info));

    return status;
}
