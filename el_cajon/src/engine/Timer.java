package engine;

public class Timer {
	private double last_loop_time;
    
    public void init() {
    	last_loop_time = getTime();
    }

    public double getTime() {
        return System.nanoTime() / 1000_000_000.0;
    }

    public float getElapsedTime() {
        double time = getTime();
        float elapsedTime = (float) (time - last_loop_time);
        last_loop_time = time;
        return elapsedTime;
    }

    public double getLastLoopTime() {
        return last_loop_time;
    }

}
