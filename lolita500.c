// LOLIcon Offended Little Idiots - TOTAL ACCELERATION to 500
// by teakhanirons, dots-tb, marburg, CelesteBlue, SilicaAndPina and others
// by CBPS: https://discord.gg/2nDCbxJ

#include <taihen.h>
#include <string.h>
#include <sys/syslimits.h>
#include <stdio.h>
#include <vitasdkkern.h>
#include <psp2/types.h>

#define HOOKS_NUM 7
static SceUID g_hooks[HOOKS_NUM];
static tai_hook_ref_t ref[HOOKS_NUM];

uint32_t *clock_speed;
unsigned int *clock_r1, *clock_r2;

static int profile_holy_shit_performance[] = {500, 222, 222, 166, 333};

unsigned int pa2va(unsigned int pa) {
	unsigned int va, vaddr, paddr, i;
	va = 0;
	for (i = 0; i < 0x100000; i++) {
		vaddr = i << 12;
		__asm__("mcr p15,0,%1,c7,c8,0\n\t"
		"mrc p15,0,%0,c7,c4,0\n\t" : "=r" (paddr) : "r" (vaddr));
		if ((pa & 0xFFFFF000) == (paddr & 0xFFFFF000)) {
			va = vaddr + (pa & 0xFFF);
			break;
		}
	}
	return va;
}

static int power_patched1(int freq) {
	int ret = 0;
	ret = TAI_CONTINUE(int, ref[0], 444);
	ksceKernelDelayThread(10000);
	*clock_speed = profile_holy_shit_performance[0];
	*clock_r1 = 0xF;
	*clock_r2 = 0x0;
	return ret;
}

static int power_patched2(int freq) {
	return TAI_CONTINUE(int, ref[1], profile_holy_shit_performance[1]);
}

static int power_patched3(int freq) {
	return TAI_CONTINUE(int, ref[2], profile_holy_shit_performance[2], profile_holy_shit_performance[2]);
}

static int power_patched4(int freq) {
	return TAI_CONTINUE(int, ref[3], profile_holy_shit_performance[3]);
}

static int config_patched(int mode) {
	ksceKernelDelayThread(10000);
	return TAI_CONTINUE(int, ref[4], 1);
}

void _start() __attribute__ ((weak, alias ("module_start")));
int module_start(SceSize argc, const void *args) {
	tai_module_info_t tai_info;
	
	tai_info.size = sizeof(tai_module_info_t);

	clock_r1 = (unsigned int *)pa2va(0xE3103000);
	clock_r2 = (unsigned int *)pa2va(0xE3103004);	
	
	taiGetModuleInfoForKernel(KERNEL_PID, "ScePower", &tai_info);
	module_get_offset(KERNEL_PID, tai_info.modid, 1,  0x4124 + 0xA4, (uintptr_t)&clock_speed);	

	kscePowerSetArmClockFrequency(profile_holy_shit_performance[0]);
	kscePowerSetBusClockFrequency(profile_holy_shit_performance[1]);
	kscePowerSetGpuEs4ClockFrequency(profile_holy_shit_performance[2], profile_holy_shit_performance[2]);
	kscePowerSetGpuXbarClockFrequency(profile_holy_shit_performance[3]);
	kscePowerSetGpuClockFrequency(profile_holy_shit_performance[4]);

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
