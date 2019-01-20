#!/usr/bin/env python3
import numpy as np
import gym
import random
import os
import math
from keras.layers import Input, Conv2D, LeakyReLU, Flatten, Dense, concatenate
from keras.models import Model
from keras.utils import to_categorical
from keras.optimizers import Adam
from collections import deque
from schedule import LinearSchedule
from replay_buffer import PrioritizedReplayBuffer, ReplayBuffer
import sys
import envs

ENV_NAME = 'FuryOfDracula-v0'
WEIGHTS_PATH = 'models/dqn_{}_dracula_weights.h5f'.format(ENV_NAME)
MEMORY = 50000
GAMMA = 0.95
SAMPLE_SIZE = 32
prioritized_replay_alpha=0.6
prioritized_replay_beta0=0.4
prioritized_replay_beta_iters=100000
eps0 = 1.0
eps_iters=200000
prioritized_replay_eps=1e-6
total_timesteps = 2500000



def create_model():
    state_input = Input(shape=(2807,))
    action_input = Input(shape=(80,))

    merged_layer = concatenate([state_input, action_input])
    dense_1 = Dense(1444)(merged_layer)
    dense_1_a = LeakyReLU()(dense_1)
    dense_2 = Dense(722)(dense_1_a)
    dense_2_a = LeakyReLU()(dense_2)
    dense_3 = Dense(361)(dense_2_a)
    dense_3_a = LeakyReLU()(dense_3)
    dense_4 = Dense(181)(dense_3_a)
    dense_4_a = LeakyReLU()(dense_4)
    output = Dense(1)(dense_4_a)

    model = Model([state_input, action_input], output)
    model.compile(optimizer=Adam(lr=0.001), loss='mse')
    model.summary()
    return model


model = create_model()

if os.path.isfile(WEIGHTS_PATH) and os.access(WEIGHTS_PATH, os.R_OK):
    model.load_weights(WEIGHTS_PATH)

# memory = deque(maxlen=MEMORY)
env = gym.make(ENV_NAME)

replay_buffer = PrioritizedReplayBuffer(MEMORY, alpha=prioritized_replay_alpha)
beta_schedule = LinearSchedule(prioritized_replay_beta_iters,
                                initial_p=prioritized_replay_beta0,
                                final_p=1.0)
eps_schedule = LinearSchedule(eps_iters,
                                initial_p=eps0,
                                final_p=0.1)


def get_action(ob, epsilon):
    moves = env.action_space
    q_values = []

    observations = np.expand_dims(ob, 0)

    for move in moves:
        action = np.zeros((80,))
        if move > 99:
            action[move - 29] = 1
        else:
            action[move] = 1
        action =  np.expand_dims(action, 0)
        inp = [observations, action]
        prediction = model.predict(inp)[0][0]
        # TODO add manual scoring to q value
        q_values.append(prediction)

    if len(q_values) == 0:
        return None

    return moves[epsgreedy_select_action(np.array(q_values), epsilon)]

def epsgreedy_select_action(q_values, epsilon):
    """Return the selected action
    # Arguments
        q_values (np.ndarray): List of the estimations of Q for each action
    # Returns
        Selection action
    """
    assert q_values.ndim == 1
    nb_actions = q_values.shape[0]

    if np.random.uniform() < epsilon:
        action = np.random.randint(0, nb_actions)
    else:
        action = np.argmax(q_values)
    return action


def get_next_max_q(observations, moves):
    q_values = []

    observations = np.expand_dims(observations, 0)

    for move in moves:
        action = np.zeros((80,))
        if move > 99:
            action[move - 29] = 1
        else:
            action[move] = 1
        action =  np.expand_dims(action, 0)
        inp = [observations, action]
        prediction = model.predict(inp)[0][0]
        q_values.append(prediction)

    if len(q_values) == 0:
        return 0

    return np.max(q_values)


def remember(ob, action, next_ob, reward, done, action_space):
    replay_buffer.add(ob, action, reward, next_ob, float(done), action_space)

def train():
    # if SAMPLE_SIZE > len(memory):
    #     return

    obes, actions, rewards, next_obes, dones, action_spaces, weights, batch_idxes = replay_buffer.sample(SAMPLE_SIZE, beta=beta_schedule.value(t))
    q_history = []
    loss_history = []
    for idx in range(len(obes)):
        ob = obes[idx]
        action = actions[idx]
        reward = rewards[idx]
        next_ob = next_obes[idx]
        done = dones[idx]
        action_space = action_spaces[idx]
        weight = weights[idx]
        # next q is enemy's best move minus your reward
        next_q = get_next_max_q(next_ob, action_space)
        q_value = reward + GAMMA * -next_q

        old_val = get_next_max_q(ob, [action])
        td_error = abs(old_val - q_value)

        ob = np.expand_dims(ob, 0)
        actionn = np.zeros((80,))
        if action > 99:
            actionn[action - 29] = 1
        else:
            actionn[action] = 1
        actionn =  np.expand_dims(actionn, 0)
        inp = [ob, actionn]

        result = model.fit(inp, [q_value], verbose=0)
        new_priority = td_error + prioritized_replay_eps
        replay_buffer.update_priority(batch_idxes[idx], new_priority)
        q_history.append(q_value)
        loss_history.append(result.history.get('loss', [])[-1])
    print('Max: {:.4f} Min: {:.4f} Mean: {:.4f}, Reward: {:.4f}, loss: {:.4f}, win: {:.4f}'
          .format(np.max(q_history), np.min(q_history), np.mean(q_history), env.rewards, np.mean(loss_history), env.win))


mode = sys.argv[1] if len(sys.argv) > 1 else ''

if mode == 'train_past_data':
    t = 0
    episodes = 0
    f = open('past_data_list.txt')
    for line in f:
        print(line)
        ob = env.reset(line[:-1])
        if env.autoHunter:
            print("Skipping")
            continue
        a = env.getSourceAction()
        while a is not None:
            t += 1
            next_ob, reward, done, info = env.step(a)
            remember(ob, a, next_ob, reward, done, env.action_space)
            ob = next_ob
            if done:
                break
            a = env.getSourceAction()
        if done == False:
            print("NOT DONE!")
        train()
        model.save_weights(WEIGHTS_PATH)
        episodes += 1
        print("Past data Episode %d done" % episodes)
        if episodes % 200 == 0:
            while True:
                ob = env.reset()
                while True:
                    a = get_action(ob, eps_schedule.value(t))
                    if a is None:
                        print("no actions!")
                        print(env.past_plays_dracula)
                        break  # no more action
                    t += 1
                    next_ob, reward, done, info = env.step(a)
                    remember(ob, a, next_ob, reward, done, env.action_space)
                    ob = next_ob
                    if done:
                        break
                train()
                print("Self-learn Episode %d done" % episodes)
                episodes += 1
                model.save_weights(WEIGHTS_PATH)
                if episodes % 100 == 0:
                    break

elif mode == 'train':

    t = 0
    while True:

        ob = env.reset()
        while True:
            a = get_action(ob, eps_schedule.value(t))

            if a is None:
                print("no actions!")
                print(env.past_plays_dracula)
                break  # no more action

            t += 1

            next_ob, reward, done, info = env.step(a)

            remember(ob, a, next_ob, reward, done, env.action_space)

            ob = next_ob

            # if mode != 'silent':
            #     env.render()

            if done:
                break

        # think
        print('%d steps passed' % t)
        # print(env.past_plays_dracula)
        if t > 1000:
            train()
        model.save_weights(WEIGHTS_PATH)
        if t > total_timesteps:
            break
elif mode == 'eval':
    t = 0
    while True:

        ob = env.reset()
        while True:
            a = get_action(ob, 0.0)

            if a is None:
                print("no actions!")
                print(env.past_plays_dracula)
                break  # no more action

            t += 1

            next_ob, reward, done, info = env.step(a)

            ob = next_ob

            if done:
                break

        # think
        print('%d steps passed' % t)
        print('reward %d win %d' % (env.rewards, env.win))
        print(env.past_plays_dracula)
        # model.save_weights(WEIGHTS_PATH)
        if t > total_timesteps:
            break
