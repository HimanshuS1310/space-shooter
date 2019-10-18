#ifndef GUARD_MANGO_ENGINE_H
#define GUARD_MANGO_ENGINE_H

#include "typedefs.h"
#include "input.h"

namespace engine
{
    class Game
    {
    public: //Typedefs
        typedef mango::shared_ptr<Game> game_handle;

    public: //Functions
        Game()
            : m_IsRunning(true),                                        m_UpdateRate(1.0 / 100),   
              m_PlayerAction(input::actions::invalid),                  m_ActionCounter(0), m_SpawnRate(2.4), 
              m_UpdateCount(0),                                         m_Score(0)        
        {
            m_Graphics = new engine::graphic_device(GetConsoleWindow(), engine::point(640, 640));
            m_Input    = new engine::input_device(input::W, input::S, input::A, input::D, input::SPACE);

            LoadSprite("player", 10, 295, 470, 50, 70); //PlayerShip Frames 10, Initial PlayerPosition (295|470), PlayerResolution 50px70p

            LoadBackground();
        }

        void Play()
        {
            srand((mango::uns32)time(nullptr));
            m_Timer.start();
            m_ShotTimer.start();
            m_SpawnTimer.start();
            m_DifficultyTimer.start();

            engine::key input = input::DEFAULT;

            while (input != input::SHIFTX && m_IsRunning)
            {
                if (mango::archive::mkbhit())
                {
                    input = input::getKeyPress();
                    KeyPressed(input);
                }

                Frame();
            }
        }

    private:

        bool SpawnCollision(mango::int32 x)
        {
            for (mango::int32 currentActor = 0; m_Actors.size() > currentActor; ++currentActor)
            {
                engine::point x1 = engine::point(x - 5 , 5);
                engine::point x2 = engine::point(x + 20, 5); 

                bool containsX1orX2 = m_Actors.at(currentActor)->Contains(x1) || m_Actors.at(currentActor)->Contains(x2);

                if (containsX1orX2)
                {
                    return true;
                }
            }

            return false;
        }

        void printScore()
        {
            //TODO:
        }

        void Frame()
        {
            Update();
            Collision();
            Draw();
            Present();
            ClearPipelines();
            SpawnEnemys();
            SetSpawnRate();
        }

        void SetSpawnRate()
        {
            const engine::seconds MINSPAWNRATE = 0.3;
            const engine::seconds INCREMENTATIONINTERVAL = 7.0;
            const engine::seconds SPAWNRATEINCRFACTOR = -0.05;

            if (MINSPAWNRATE <= m_SpawnRate)
            {
                m_DifficultyTimer.tick();
                if (INCREMENTATIONINTERVAL <= m_DifficultyTimer.totalTime())
                {
                    m_SpawnRate += SPAWNRATEINCRFACTOR;
                    ResetTimer(m_DifficultyTimer);
                }
            }
        }

        void SelectEnemyType(mango::int32 x)
        {
            mango::int32 enemyType = rand() % 6 + 64; //A-E
            std::string name = "enemy";
            name.push_back(enemyType);

            if ('A' == enemyType || 'C' == enemyType)
            {
                LoadEnemy(name, 1, x, -5, 40, 40); //40 - 40 --> enemyA and enemyC sprite are at the same resolution --> 40px40p 
            }
            if ('B' == enemyType || 'E' == enemyType)
            {
                LoadEnemy(name, 1, x, -5, 25, 50); //25 - 50 --> enemyB and enemyE sprite are at the same resolution --> 25px50p 
            }
            if ('D' == enemyType)
            {
                LoadEnemy(name, 1, x, -5, 30, 54); //30 - 54 --> enemyD --> 30px54p 
            }
        }

        void SpawnEnemys()
        {
            const mango::int32 MAXACTORS = 50;

            m_SpawnTimer.tick();
            if (m_SpawnRate <= m_SpawnTimer.totalTime() &&m_Actors.size() < MAXACTORS)
            {
                ResetTimer(m_SpawnTimer);

                mango::int32 x = 0;

                do
                {
                    x = rand() % 550 + 5; //5-550 Spawn Frame
                } while (SpawnCollision(x));

                SelectEnemyType(x);             
            }
        }

        static bool ProcessCollision(engine::actor_handle shot, engine::sprite_pipeline* actors, engine::actor_pipeline* shots)
        {
            const mango::int32 MAXENEMY_Y = 650;
            const mango::int32 MAXSHOT_Y = -10;

            for (mango::int32 currentActor = 0; actors->size() > currentActor; ++currentActor)
            {
                engine::point x1 = shot->GetPosition();
                engine::point x2 = shot->GetPosition();

                x2.X += 10;

                bool bothAreVisible = shot->GetVisibility() && actors->at(currentActor)->GetVisibility();
                bool containsX1orX2 = actors->at(currentActor)->Contains(x1) || actors->at(currentActor)->Contains(x2);
                bool isNoOtherShot  = 0 == shots->find(actors->at(currentActor));

                if(bothAreVisible && containsX1orX2 && isNoOtherShot) //Remove hit and ship if hit
                {
                    actors->at(currentActor)->SetVisibility(false);
                    shot->SetVisibility(false);
                }
                if (MAXENEMY_Y <= actors->at(currentActor)->GetPosition().Y) //Remove ship if out of screen
                {
                    actors->at(currentActor)->SetVisibility(false);
                }
                if (MAXSHOT_Y >= x1.Y) //Remove shot if out of screen
                {
                    shot->SetVisibility(false);
                }
            }

            return true;
        }

        void Collision()
        {
            for (mango::int32 currentShot = 0; m_Shots.size() > currentShot; ++currentShot)
            {
                ProcessCollision(m_Shots.at(currentShot), &m_Actors, &m_Shots);
            }
        }

        void ClearPipelines()
        {
            for (mango::int32 currentShot = 0; m_Shots.size() > currentShot; ++currentShot)
            {
                if (!m_Shots.at(currentShot)->GetVisibility())
                {
                    m_Actors.remove(m_Shots.at(currentShot));
                    m_Shots.erase(currentShot);

                    --currentShot;
                }
            }

            for (mango::int32 currentActor = 0; m_Actors.size() > currentActor; ++currentActor)
            {
                const mango::int32 MAXENEMY_Y = 650;

                if (MAXENEMY_Y <= m_Actors.at(currentActor)->GetPosition().Y)
                {
                    engine::actor_handle gameOver = new engine::actor(engine::point(0, 150), 600, 200);
                    gameOver->LoadAnimation(0, "game_over", 1);

                    m_Graphics->ClearBuffer();
                    m_Graphics->Draw((engine::actor*)m_Background);
                    m_Graphics->Draw((engine::actor*)gameOver);
                    m_Graphics->Present();

                    mango::csleep(3000); //3 Seconds

                    m_IsRunning = false;
                    return;
                }

                if (!m_Actors.at(currentActor)->GetVisibility())
                {
                    SpawnEffect(m_Actors.at(currentActor), "explosion");
                    m_Actors.remove(m_Actors.key_at(currentActor));

                    --currentActor;
                    m_Score += 100;
                }
            }
        }

        void Shoot(engine::key key)
        {
            const mango::int32 ACTIONCOUNTERINCRFACTOR = 30;

            if (input::actions::shoot == m_Input->KeyPressed(key))
            {
                SpawnShot("player");

                if ((0 != m_ActionCounter && input::actions::right == m_PlayerAction))
                {
                    m_PlayerAction = input::actions::right;
                    m_ActionCounter = ACTIONCOUNTERINCRFACTOR;
                }
                if ((0 != m_ActionCounter && input::actions::left == m_PlayerAction))
                {
                    m_PlayerAction = input::actions::left;
                    m_ActionCounter = ACTIONCOUNTERINCRFACTOR;
                }
                if ((0 != m_ActionCounter && input::actions::up == m_PlayerAction))
                {
                    m_PlayerAction = input::actions::up;
                    m_ActionCounter = ACTIONCOUNTERINCRFACTOR;
                }
                if ((0 != m_ActionCounter && input::actions::down == m_PlayerAction))
                {
                    m_PlayerAction = input::actions::down;
                    m_ActionCounter = ACTIONCOUNTERINCRFACTOR;
                }
            }
        }

        void Move(engine::key key)
        {
            const mango::int32 ACTIONCOUNTERINCRFACTOR = 30;

            if (input::actions::left == m_Input->KeyPressed(key))
            {
                if (0 == m_ActionCounter || input::actions::left != m_PlayerAction)
                {
                    m_PlayerAction = input::actions::left;
                    m_ActionCounter = ACTIONCOUNTERINCRFACTOR;
                }
            }
            if (input::actions::right == m_Input->KeyPressed(key))
            {
                if (0 == m_ActionCounter || input::actions::right != m_PlayerAction)
                {
                    m_PlayerAction = input::actions::right;
                    m_ActionCounter = ACTIONCOUNTERINCRFACTOR;
                }
            }
            if (input::actions::up == m_Input->KeyPressed(key))
            {
                if (0 == m_ActionCounter || input::actions::up != m_PlayerAction)
                {
                    m_PlayerAction = input::actions::up;
                    m_ActionCounter = ACTIONCOUNTERINCRFACTOR;
                }
            }
            if (input::actions::down == m_Input->KeyPressed(key))
            {
                if (0 == m_ActionCounter || input::actions::down != m_PlayerAction)
                {
                    m_PlayerAction = input::actions::down;
                    m_ActionCounter = ACTIONCOUNTERINCRFACTOR;
                }
            }
        }

        void KeyPressed(engine::key key)
        {
            Shoot(key); 
            Move(key);
        }


        void Draw()
        {
            m_Graphics->ClearBuffer();

            m_Graphics->Draw((engine::actor*)m_Background);
            m_Graphics->Draw(m_Actors);

            for (mango::int32 currentEffect = 0; m_Effects.size() > currentEffect; ++currentEffect)
            {
                m_Graphics->Draw((engine::actor*)m_Effects.key_at(currentEffect));
            }
        }

        void UpdateEnemies()
        {
            const mango::int32 ENEMYMOTIONRATE_X = 0;
            const mango::int32 ENEMYMOTIONRATE_Y = 1;

            for (mango::int32 currentActor = 0; m_Actors.size() > currentActor; ++currentActor)
            {
                bool isNoPlayerShot = 0 == m_Shots.find(m_Actors.at(currentActor));
                bool isNotThePlayer = m_Actors.at(currentActor) != m_Actors.at(0);

                if (isNoPlayerShot && isNotThePlayer)
                {
                    m_Actors.at(currentActor)->Move(ENEMYMOTIONRATE_X, ENEMYMOTIONRATE_Y);
                }
            }
        }

        void UpdateActors()
        {
            m_Timer.tick();
            if ((m_UpdateRate) <= m_Timer.totalTime())
            {
                if (0 != m_Actors.size())
                {
                    lambda((engine::actor_handle actor) { actor->Update(); return 0; }, 1); //exp1
                    m_Actors.for_each(exp1);
                }

                ++m_UpdateCount;

                UpdatePlayer();
                UpdateEffects();
                UpdateEnemies();

                ResetTimer(m_Timer);
            }
        }

        void UpdateBackground()
        {
            const mango::int32 BGMOTIONRATE_X = 0;
            const mango::int32 BGMOTIONRATE_Y = 1;

            const mango::int32 BGUPDATETHRESHHOLD = 3;

            if (BGUPDATETHRESHHOLD == m_UpdateCount)
            {
                if (nullptr != m_Background)
                {
                    m_Background->Move(BGMOTIONRATE_X, BGMOTIONRATE_Y);

                    if (0 == m_Background->GetPosition().Y)
                    {
                        m_Background->SetPosition(engine::point(0, -600)); //Reset BG to initial position
                    }
                }

                m_UpdateCount = 0;
            }
        }

        void UpdateShots()
        {
            const mango::int32 SHOTMOTIONRATE_X = 0;
            const mango::int32 SHOTMOTIONRATE_Y = -4;

            if (m_Shots.size() > 0)
            {
                lambda((engine::actor_handle shot) { shot->Move(SHOTMOTIONRATE_X, SHOTMOTIONRATE_Y); return 0; }, 1);

                m_Shots.for_each(exp1);
            }
        }

        void UpdateEffects()
        {
            for (mango::int32 currentEffect = 0; m_Effects.size() > currentEffect; ++currentEffect)
            {       
                m_Effects.key_at(currentEffect)->Update();

                if (mango::npos == --m_Effects.at(currentEffect))
                {
                    m_Effects.remove(m_Effects.key_at(currentEffect));
                    --currentEffect;
                }
            }
        }

        void UpdatePlayer()
        {
            const mango::int32 PLAYERMOTIONRATE = 3;
            const mango::int32 NOMOTION = 0;

            const mango::int32 MINPLAYER_X = 5;
            const mango::int32 MAXPLAYER_X = 550;

            const mango::int32 MINPLAYER_Y = 5;
            const mango::int32 MAXPLAYER_Y = 470;

            if (0 != m_ActionCounter)
            {
                engine::point player = m_Actors.at(0)->GetPosition();

                if (input::actions::left == m_PlayerAction)
                {
                    if (player.X >= MINPLAYER_X)
                    {
                        MovePlayer( -PLAYERMOTIONRATE, NOMOTION);
                    }
                    --m_ActionCounter;
                }
                if (input::actions::right == m_PlayerAction)
                {
                    if (player.X <= MAXPLAYER_X)
                    {
                        MovePlayer(PLAYERMOTIONRATE, NOMOTION);
                    }
                    --m_ActionCounter;
                }
                if (input::actions::up == m_PlayerAction)
                {
                    if (player.Y >= MINPLAYER_Y)
                    {
                        MovePlayer(NOMOTION, -PLAYERMOTIONRATE);
                    }
                    --m_ActionCounter;
                }
                if (input::actions::down == m_PlayerAction)
                {
                    if (player.Y <= MAXPLAYER_Y)
                    {
                        MovePlayer(NOMOTION, PLAYERMOTIONRATE);
                    }
                    --m_ActionCounter;
                }
            }
            else
            {
                m_PlayerAction = input::actions::invalid;
            }
        }

        void Update()
        {
            UpdateActors();
            UpdateBackground();
            UpdateShots();
        }

        void Present()
        {
            m_Graphics->Present();
            printScore();
        }

        void ResetTimer(engine::timer& timer)
        {
            timer.stop();
            timer.reset();
            timer.start();
        }

        void LoadSprite(const engine::mesh_name& name, mango::int32 frames, mango::int32 x, mango::int32 y, mango::int32 width, mango::int32 height)
        {
            engine::actor_handle actor;

            actor = new engine::actor(engine::point(x, y), width, height);
            actor->LoadAnimation(0, name, frames);

            m_Actors.add(name, actor);
        }

        void LoadEnemy(const engine::mesh_name& name, mango::int32 frames, mango::int32 x, mango::int32 y, mango::int32 width, mango::int32 height)
        {
            static engine::large_counter ID = 0;

            engine::actor_handle actor;

            actor = new engine::actor(engine::point(x, y), width, height);
            actor->LoadAnimation(0, name, frames);

            m_Actors.add(name + mango::to_string(ID), actor);

            ++ID;
        }

        void LoadShot(const engine::mesh_name& name, mango::int32 frames, mango::int32 x, mango::int32 y, mango::int32 width, mango::int32 height)
        {
            static engine::large_counter ID = 0;

            engine::actor_handle actor;

            actor = new engine::actor(engine::point(x, y), width, height);
            actor->LoadAnimation(0, name, frames);

            m_Shots.push_back(actor);
            m_Actors.add(name + mango::to_string(ID), actor);

            ++ID;
        }

        void LoadEffect(const engine::mesh_name& name, mango::int32 frames, mango::int32 x, mango::int32 y, mango::int32 width, mango::int32 height)
        {
            const mango::int32 EFFECTFRAMES = 5;

            engine::actor_handle actor;

            actor = new engine::actor(engine::point(x, y), width, height);
            actor->LoadAnimation(0, name, frames);

            m_Effects.add(actor, EFFECTFRAMES);
        }

        void LoadBackground()
        {
            m_Background = new engine::actor(engine::point(0, -600), 640, 1200);
            m_Background->LoadAnimation(0, "background", 1);
        }

        void MoveActor(std::string name, mango::int32 x, mango::int32 y)
        {
            m_Actors.at(name)->Move(x, y);
        }

        void MovePlayer(mango::int32 x, mango::int32 y)
        {
            m_Actors.at(0)->Move(x, y);
        }

        void SpawnShot(std::string actor_name)
        {
            m_ShotTimer.tick();

            if (0.20 <= m_ShotTimer.totalTime())
            {
                ResetTimer(m_ShotTimer);

                engine::point spawnPosition = m_Actors.at(actor_name)->GetPosition();

                if ("player" == actor_name)
                {
                    spawnPosition.X += 22;
                    spawnPosition.Y -= 25;

                    LoadShot("playerShot", 1, spawnPosition.X, spawnPosition.Y, 10, 20);
                }
            }
        }

        void SpawnEffect(actor_handle actor, std::string effect_name)
        {
            engine::point spawnPosition = actor->GetPosition();

            LoadEffect(effect_name, 5, spawnPosition.X, spawnPosition.Y, 50, 50);
        }

        void SpawnEffect(std::string actor_name, std::string effect_name)
        {
            engine::point spawnPosition = m_Actors.at(actor_name)->GetPosition();

            LoadEffect(effect_name, 5, spawnPosition.X, spawnPosition.Y, 50, 50);
        }

        input::action             m_PlayerAction;
        engine::counter           m_ActionCounter; //While Counter != 0 PlayerAction (except shooting) will be repeated for control smoothing

        engine::seconds           m_UpdateRate;
        engine::seconds           m_SpawnRate;
        engine::counter           m_UpdateCount;

        engine::timer             m_Timer;
        engine::timer             m_ShotTimer;
        engine::timer             m_SpawnTimer;
        engine::timer             m_DifficultyTimer;

        engine::sprite_pipeline   m_Actors;
        engine::graphic_handle    m_Graphics;
        engine::input_handle      m_Input;

        engine::actor_pipeline    m_Shots;
        engine::effect_pipeline   m_Effects;

        engine::background_handle m_Background;

        engine::flag              m_IsRunning;
        engine::counter           m_Score;
    };

    typedef mango::pair<engine::Game::game_handle, engine::key> input_package;
}


#endif//GUARD_MANGO_ENGINE_H
