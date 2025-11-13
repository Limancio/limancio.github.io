package engine;

import java.nio.IntBuffer;
import java.nio.ShortBuffer;

import static org.lwjgl.openal.AL10.*;

import org.lwjgl.BufferUtils;
import org.lwjgl.stb.STBVorbis;

public class Sound {
	public int buffer_id;
	public int source_id;
	public String path;
	
	public boolean is_playing;

	public void
	load_sound_from_file(String path, boolean play_in_loop) {
		this.path = path;
		

		IntBuffer channels_buffer = BufferUtils.createIntBuffer(1);
		IntBuffer sample_rate_buffer = BufferUtils.createIntBuffer(1);
		ShortBuffer raw_audio_buffer = STBVorbis.stb_vorbis_decode_filename(path, channels_buffer, sample_rate_buffer);
		if(raw_audio_buffer == null) {
			System.err.println("Clould not load sound [" + path + "].");
			return;
		}
		
		int channels = channels_buffer.get();
		int sample_rate = sample_rate_buffer.get();
		
		int format = -1;
		if(channels == 1) {
			format = AL_FORMAT_MONO16;
		} else if(channels == 2) {
			format = AL_FORMAT_STEREO16;
		}
		
		buffer_id = alGenBuffers();
		alBufferData(buffer_id, format, raw_audio_buffer, sample_rate);
		
		source_id = alGenSources();
		alSourcei(source_id, AL_BUFFER,  buffer_id);
		alSourcei(source_id, AL_LOOPING, (play_in_loop) ? 1 : 0);
		alSourcei(source_id, AL_POSITION, 0);
		alSourcef(source_id, AL_GAIN, 0.3f);
	}

	public void force_play() {
		int state = alGetSourcei(source_id, AL_SOURCE_STATE);
		
		if(state == AL_STOPPED) {
			is_playing = false;
			alSourcei(source_id, AL_POSITION, 0);
		}

		alSourcePlay(source_id);
		is_playing = true;
	}
	
	public void play() {
		int state = alGetSourcei(source_id, AL_SOURCE_STATE);
		
		if(state == AL_STOPPED) {
			is_playing = false;
			alSourcei(source_id, AL_POSITION, 0);
		}
		
		if(!is_playing) {
			alSourcePlay(source_id);
			is_playing = true;
		}
	}
	
	public void stop() {
		if(is_playing) {
			alSourceStop(source_id);
			is_playing = false;
		}
	}
	
	public void
	cleanup() {
		alDeleteSources(source_id);
		alDeleteBuffers(buffer_id);
	}
	
}
