//* Discobots 1104A comp code.
//* Marco Tan, Neil Sachdeva, Dev Patel
//*
//* File Created: 2020-09-26
//* Desc: Main autonomous control code.


//* Headers
#include "main.h"   // Main header.


//* Defines for testing purposes.
#define SECTION_ONE

//* Function declarations for the tasks.

void intake_until_in();


//* Local "global" objects.

// PID gains for straight movements.
a_PID_Gains gains_str {
    k_Auto::a_def_kP, k_Auto::a_def_kI, k_Auto::a_def_kD, 
    k_Hardware::h_max_readtime, k_Auto::a_def_integ_windup, 
    k_Auto::a_def_ocr_tick_range, k_Auto::a_def_imu_head_range};

// PID gains for point turns.
a_PID_Gains gains_p_trn {
    k_Auto::a_p_trn_kP, k_Auto::a_p_trn_kI, k_Auto::a_p_trn_kD, 
    k_Hardware::h_max_readtime, k_Auto::a_def_integ_windup, 
    k_Auto::a_def_ocr_tick_range, k_Auto::a_def_imu_head_range
};

// Task for intake_until_in()
pros::Task t_intake_until_in {intake_until_in};

//* Macro functions

// This autonomous macro is for scoring a ball.
void score()
{
    // Infinite loop.
    while (true)
    {
        // Look for a ball with the specified signature, red.
        pros::vision_object_s_t ball {h_obj_sensors->get_obj_sig(0, h_sVision_IDs::RED_ID)};
        
        // If ball width is greater than 200...
        if (ball.width > 200)
        {
            // Wait 2 seconds, then cut power to the conveyors and exit the loop.
            pros::delay(2000);
            h_obj_conveyor->set_vel();
            break;
        }

        // Otherwise, just keep running the conveyors.
        h_obj_conveyor->set_vel(600);
        pros::delay(k_Hardware::h_max_readtime);
    }
}

// This autonomous macro is for descoring the balls.
void descore()
{
    // Infinite loop 1.
    while (true)
    {
        // Look for a ball with the specified size.
        pros::vision_object_s_t ball {h_obj_sensors->get_obj_siz(0)};

        // If the signature is red and the width is greater than 50...
        if (ball.signature == static_cast<int>(h_sVision_IDs::RED_ID) && ball.width > 50)
        {
            // Cut power to the intakes and exit the loop.
            h_obj_intake->set_vel();
            break;
        }

        // Otherwise, eject the opponent balls.
        h_obj_conveyor->set_vel((ball.width > 50) ? -600 : 600, 600);
        h_obj_intake->set_vel(600);
        pros::delay(k_Hardware::h_max_readtime);
    }

    // Infinite loop 2.
    while (true)
    {
        // Look for a ball with the specified signature, red.
        pros::vision_object_s_t ball {h_obj_sensors->get_obj_sig(0, h_sVision_IDs::RED_ID)};
        
        // If ball width is greater than 200...
        if (ball.width > 200)
        {
            // Wait 2 seconds, then cut power to the conveyors and exit the loop.
            pros::delay(2000);
            h_obj_conveyor->set_vel();
            break;
        }

        // Otherwise, just keep running the conveyors.
        h_obj_conveyor->set_vel(600);
        pros::delay(k_Hardware::h_max_readtime);
    }
}

// This autonomous macro is for intaking the balls as we drive.
void intake_until_in()
{
    // If the task is notified...
    while (t_intake_until_in.notify_take(true, TIMEOUT_MAX))
    {
        // Infinite loop.
        while (true)
        {
            // Look for a ball with the specified signature, red.
            pros::vision_object_s_t ball {h_obj_sensors->get_obj_sig(0, h_sVision_IDs::RED_ID)};
            
            // If ball width is greater than 100...
            if (ball.width > 100)
            {
                // Cut conveyor and intake power immediately and exit the loop.
                h_obj_conveyor->set_vel();
                h_obj_intake->set_vel();
                break;
            }

            // Otherwise, just keep running the conveyors and intake.
            h_obj_conveyor->set_vel(600);
            h_obj_intake->set_vel(600);
            pros::delay(k_Hardware::h_max_readtime);
        }
    }
}


//* Routine functions.

// Red routine.
void red()
{

}

// Blue routine.
void blue()
{

}

// Skills routine.
void skills()
{
    //--START OF SECTION ONE--//
    // This part just scores the preload ball into Goal I.
    // It immediately grants us 19 points:
    //   - 3 descored rows * 6.
    //   - 1 alliance ball scored.

    // Reset the encoders.
    h_obj_sensors->reset_enc();

    // Score the preload ball in.
    // The use of the Vision sensor is to make sure that the ball does go in.
    score();

    // Back out by a foot or so.
    a_obj_pid->set_target(a_Ticks{-1.0_ft}).drive();
    pros::delay(50);

    // Turn to 90 degrees heading relative to our starting position
    a_obj_pid->set_gains(gains_p_trn).set_target(a_Degrees{90.0}).drive();
    pros::delay(50);

#if defined SECTION_ONE
    return;
#endif
    //---END OF SECTION ONE---//

    //--START OF SECTION TWO--//
    // This part drives the robot to Goal F and scores an alliance ball.
    // It bumps our score up to 26 points.
    //  - 1 descored row * 6.
    //  - 1 alliance ball scored.

    // Drive forward for about 4 feet.
    // Turn on the intakes and conveyor too so we can intake the ball.
    t_intake_until_in.notify();
    a_obj_pid->set_target(a_Ticks{4.0_ft}).drive();
    pros::delay(50);

    // Turn to 180 degrees heading relative to our starting position
    a_obj_pid->set_gains(gains_p_trn).set_target(a_Degrees{180.0}).drive();
    pros::delay(50);

    // Drive forward by a foot or so.
    a_obj_pid->set_target(a_Ticks{1.0_ft}).drive();
    pros::delay(50);

    // Score the preload ball in.
    // The use of the Vision sensor is to make sure that the ball does go in.
    score();

    // Back out by a foot or so.
    a_obj_pid->set_target(a_Ticks{-1.0_ft}).drive();
    pros::delay(50);

    // Turn to 90 degrees heading relative to our starting position
    a_obj_pid->set_gains(gains_p_trn).set_target(a_Degrees{90.0}).drive();
    pros::delay(50);

#if defined SECTION_TWO
    return;
#endif
    //---END OF SECTION TWO---//

    //--START OF SECTION THREE--//

#if defined SECTION_THREE
    return;
#endif
    //---END OF SECTION THREE---//
}

// Main autonomous control callback.
void autonomous()
{
    // Create new a_PID object into heap memory.
    a_obj_pid = new a_PID{gains_str};
    
    // Run each auto routine based on selected auto.
    switch (a_routine)
    {
    case a_Autonomous_Routine::RED:
        red();
        break;
    case a_Autonomous_Routine::BLUE:
        blue();
        break;
    case a_Autonomous_Routine::SKILLS:
        skills();
        break;
    }
}
