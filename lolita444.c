// LOLIcon Offended Little Idiots - tiny acceleration to 444
// by teakhanirons, dots-tb, marburg, CelesteBlue, SilicaAndPina and others
// by CBPS: https://discord.gg/2nDCbxJ

#include <taihen.h>
#include <string.h>
#include <sys/syslimits.h>
#include <stdio.h>
#include <vitasdkkern.h>
#include <psp2/types.h>

#define HOOKS_NUM 5
static SceUID g_hooks[HOOKS_NUM];
static tai_hook_ref_t ref[HOOKS_NUM];

static int profile_max_performance[] = {444, 222, 222, 166, 333};

static int power_patched1(int freq) {
	return TAI_CONTINUE(int, ref[0], profile_max_performance[0]);
}

static int power_patched2(int freq) {
	return TAI_CONTINUE(int, ref[1], profile_max_performance[1]);
}

static int power_patched3(int freq) {
	return TAI_CONTINUE(int, ref[2], profile_max_performance[2], profile_max_performance[2]);
}

static int power_patched4(int freq) {
	return TAI_CONTINUE(int, ref[3], profile_max_performance[3]);
}

static int config_patched(int mode) {
	ksceKernelDelayThread(10000);
	return TAI_CONTINUE(int, ref[4], 1);
}

void _start() __attribute__ ((weak, alias ("module_start")));
int module_start(SceSize argc, const void *args) {
	kscePowerSetArmClockFrequency(profile_max_performance[0]);
	kscePowerSetBusClockFrequency(profile_max_performance[1]);
	kscePowerSetGpuEs4ClockFrequency(profile_max_performance[2], profile_max_performance[2]);
	kscePowerSetGpuXbarClockFrequency(profile_max_performance[3]);
	kscePowerSetGpuClockFrequency(profile_max_performance[4]);

	g_hooks[0] = taiHookFunctionExportForKernel(KERNEL_PID, &ref[0], "ScePower", 0x1590166F, 0x74DB5AE5, power_patched1);
	g_hooks[1] = taiHookFunctionExportForKernel(KERNEL_PID,	&ref[1], "ScePower", 0x1590166F, 0xB8D7B3FB, power_patched2);
	g_hooks[2] = taiHookFunctionExportForKernel(KERNEL_PID, &ref[2], "ScePower", 0x1590166F, 0x264C24FC, power_patched3);
	g_hooks[3] = taiHookFunctionExportForKernel(KERNEL_PID, &ref[3], "ScePower", 0x1590166F, 0xA7739DBE, power_patched4);
	g_hooks[4] = taiHookFunctionExportForKernel(KERNEL_PID, &ref[4], "ScePower", TAI_ANY_LIBRARY, 0x3CE187B6, config_patched);

	return SCE_KERNEL_START_SUCCESS;
}

int module_stop(SceSize argc, const void *args) {
	for(int i = 0; i < HOOKS_NUM; i++) {
		if (g_hooks[i] >= 0) taiHookReleaseForKernel(g_hooks[i], ref[i]);
	}

	return SCE_KERNEL_STOP_SUCCESS;
}
