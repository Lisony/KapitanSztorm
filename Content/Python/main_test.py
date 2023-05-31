import os
from unreal.mladapter.envs import *
from unreal.mladapter.utils import random_action, DEFAULT_PORT
import unreal.mladapter.logger as logger
import argparse
import numpy as np
from policy_test import Agent

# from utils import base_path, plot_learning_curve


if __name__ == '__main__':
    # ----------------------
    env_cls = KapitanSztorm
    logger.set_level(logger.DEBUG)
    parser = argparse.ArgumentParser()
    parser.add_argument('--port', type=int, default=DEFAULT_PORT)
    args = parser.parse_args()
    env = env_cls(ue_params=None, server_port=args.port)
    # ----------------------
    fc1 = 64
    fc2 = 64
    input_dim = np.prod(env.observation_space.shape)

    print("Action space shape: ",env.action_space.shape[0])
    agent = Agent(input_dims=env.observation_space.shape, env=env, n_actions=env.action_space.shape[0],
                  fc1=fc1, fc2=fc2)
    n_games = 100

    # figure_file = os.path.join(base_path(), 'plots\\ue_pendulum_'+str(fc1)+'x'+str(fc2)+'_'+str(n_games)+'.png')

    best_score = env.reward_range[0]
    score_history = []
    load_checkpoint = False

    if load_checkpoint:
        n_steps = 0
        while n_steps <= agent.batch_size:
            observation = env.reset()
            action = env.action_space.sample()
            observation_, reward, done, info = env.step(action)
            agent.remember(observation, action, reward, observation_, done)
            n_steps += 1
        agent.learn()
        # agent.load_models()
        evaluate = True
    else:
        evaluate = False



    for i in range(n_games):
        observation = env.reset()
        done = False
        score = 0
        while not done:
            action = np.array(agent.choose_action(observation, evaluate))
            action = np.clip(action, -1.0, 1.0)
            # print(f"Action: {action}")
            action = action.astype(np.float32)
            observation_, reward, done, info = env.step(action)
            # print(reward)
            score += reward
            agent.remember(observation, action, reward, observation_, done)
            if not load_checkpoint:
                agent.learn()
            observation = observation_

        score_history.append(score)
        avg_score = np.mean(score_history[-100:])

        if avg_score > best_score:
            best_score = avg_score
            if not load_checkpoint:
                # agent.save_models()
                pass

        print('episode ', i, 'score %.1f' % score, 'avg score %.1f' % avg_score)

    # if not load_checkpoint:
    #     x = [i+1 for i in range(n_games)]
    #     plot_learning_curve(x, score_history, figure_file)