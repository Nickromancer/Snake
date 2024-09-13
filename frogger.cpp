#include <iostream>
#include <chrono>
#include <random>
#include <map>
#include <tuple>
#include <string>

#include "ITUGames.h"

#pragma region Engine State

int frame = 0;

struct ScreenBuffer
{
    std::map<std::tuple<int, int>, char> buffer;
};

struct Player
{
    std::tuple<int, int> position;
    std::tuple<int, int> prev_position;
    int playerAnimationFrame;
    unsigned char input = -1;

    void Init(int start_x = 20, int start_y = 20)
    {
        position = {start_x, start_y};
    }

    bool ProcessEvent(char p_input)
    {
        input = p_input;
        return false;
    }

    void Update(int &frame)
    {
        frame++;

        prev_position = position;

        if (input == ITUGames::Console::KEY_W)
            std::get<1>(position)--;
        if (input == ITUGames::Console::KEY_S)
            std::get<1>(position)++;
        if (input == ITUGames::Console::KEY_A)
            std::get<0>(position)--;
        if (input == ITUGames::Console::KEY_D)
            std::get<0>(position)++;

        playerAnimationFrame = (frame / 30) % 2;
    }

    void Render()
    {
        if (std::get<0>(position) != std::get<0>(prev_position) || std::get<1>(position) != std::get<1>(prev_position))
            ITUGames::Console::RenderCharacter(' ', std::get<0>(prev_position), std::get<1>(prev_position));
        ITUGames::Console::RenderCharacter(playerAnimationFrame ? '0' : 'O', std::get<0>(position), std::get<1>(position));
    }
};

Player player;

struct Score
{
    int wins, deaths;

    void Init()
    {
        wins, deaths = 0;
    }
    void Update()
    {
        return;
    }
    void Render()
    {

        // score
        std::cout << " Wins   : " << wins << std::endl;
        std::cout << " Deaths : " << deaths << std::endl;
        std::cout << " Frame   : " << frame << std::endl;
    }
};

Score score;

struct Engine
{
    std::chrono::time_point<std::chrono::steady_clock> time_start;
    std::chrono::time_point<std::chrono::steady_clock> time_end;
    std::chrono::time_point<std::chrono::steady_clock> time_end_computation;
    std::chrono::duration<double> time_elapsed;
    std::chrono::duration<double> time_computation;

    bool b_show_debug_window;

    ScreenBuffer currentBuffer;
    ScreenBuffer nextBuffer;

    const std::chrono::duration<double> MAX_FRAME_TIME = std::chrono::duration<double>(1 / 60.0);

    void GetFPS();
    void GetTimeElapsedMs();
    void GetTimeComputation();
    void Init()
    {
        ITUGames::Console::InitScreenForRendering();
        ITUGames::Console::HideCursor();

        player.Init();
        score.Init();
    }

    bool ProcessEvent()
    {
        unsigned char input = ITUGames::Console::GetCharacter(false);

        player.ProcessEvent(input);
        return false;
    }

    void Update()
    {
        if (player.input == ITUGames::Console::KEY_1)
        {
            b_show_debug_window = !b_show_debug_window;

            if (!b_show_debug_window)
                // do a full clear to hide the panel
                ITUGames::Console::ClearScreen();
        }

        player.Update(frame);
        score.Update();
        std::cout << std::flush;
    }

    void Render()
    {
        ITUGames::Console::GotoTop();

        if (b_show_debug_window)
        // debug
        {
            std::cout << " Target(ms)      : " << 1000 * MAX_FRAME_TIME.count() << std::endl;
            std::cout << " Computation(ms) : " << 1000 * time_computation.count() << std::endl;
            std::cout << " Elapsed(ms)     : " << 1000 * time_elapsed.count() << std::endl;
            std::cout << " FPS             : " << 1.0 / time_elapsed.count() << std::endl;
        }
        else
            score.Render();

        player.Render();
    }
};

Engine engine;

int main()
{
    engine.Init();

    while (true)
    {
        engine.time_start = std::chrono::steady_clock::now();

        engine.ProcessEvent();
        engine.Update();
        engine.Render();

        engine.time_end_computation = std::chrono::steady_clock::now();
        engine.time_computation = engine.time_end_computation - engine.time_start;

        ITUGames::Utils::PreciseSleep(engine.MAX_FRAME_TIME - engine.time_computation);

        engine.time_end = std::chrono::steady_clock::now();
        engine.time_elapsed = engine.time_end - engine.time_start;
    }
    return 0;
}
