package core;

import engine.Engine;

public class Main {

	public static void main(String[] args) {
        Engine game_engine = new Engine("Equipo de Programación", 1280, 720, true);
        game_engine.init();
        game_engine.run();
	}
}
