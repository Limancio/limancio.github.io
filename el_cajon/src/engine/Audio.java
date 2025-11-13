package engine;

import org.lwjgl.openal.AL;
import org.lwjgl.openal.ALC;
import org.lwjgl.openal.ALC10;
import org.lwjgl.openal.ALCCapabilities;
import org.lwjgl.openal.ALCapabilities;
import org.lwjgl.system.MemoryUtil;

import java.nio.IntBuffer;

public class Audio {
	public long context;
	public long device;
	public ALCCapabilities alc_capabilities;
	public ALCapabilities al_capabilities;
	
	public void init() {
		String defaultDeviceName = ALC10.alcGetString(0, ALC10.ALC_DEFAULT_DEVICE_SPECIFIER);
		device = ALC10.alcOpenDevice(defaultDeviceName);

		context = ALC10.alcCreateContext(device, (IntBuffer) null);
		ALC10.alcMakeContextCurrent(context);

		alc_capabilities = ALC.createCapabilities(device);
		al_capabilities  = AL.createCapabilities(alc_capabilities);
		
		if(!al_capabilities.OpenAL10) {
			System.err.println("Audio library not suported.");
		}
	}
	
	public void cleanup() {
		ALC10.alcMakeContextCurrent(MemoryUtil.NULL);
		ALC10.alcDestroyContext(context);
		ALC10.alcCloseDevice(device);
	}
}
