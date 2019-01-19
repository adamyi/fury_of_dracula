#!/usr/bin/env python3
import numpy as np
import gym
import random
import os
import math
from keras.layers import Input, Conv2D, LeakyReLU, Flatten, Dense, concatenate
from keras.models import Model
from keras.utils import to_categorical
from collections import deque
import sys
import envs

ENV_NAME = 'FuryOfDracula-v0'
WEIGHTS_PATH = 'models/dqn_{}_dracula_weights.h5f'.format(ENV_NAME)
MEMORY = 1000
GAMMA = 0.95
SAMPLE_SIZE = 32
EXPLORE_RATE = 0.1


def create_model():
    state_input = Input(shape=(392,))
    action_input = Input(shape=(1,))

    merged_layer = concatenate([state_input, action_input])
    dense_1 = Dense(196)(merged_layer)
    dense_1_a = LeakyReLU()(dense_1)
    dense_2 = Dense(196)(dense_1_a)
    dense_2_a = LeakyReLU()(dense_2)
    dense_3 = Dense(98)(dense_2_a)
    dense_3_a = LeakyReLU()(dense_3)
    dense_4 = Dense(98)(dense_3_a)
    dense_4_a = LeakyReLU()(dense_4)
    output = Dense(1)(dense_4_a)

    model = Model([state_input, action_input], output)
    model.compile('adam', loss='mse')
    model.summary()
    return model


model = create_model()

if os.path.isfile(WEIGHTS_PATH) and os.access(WEIGHTS_PATH, os.R_OK):
    model.load_weights(WEIGHTS_PATH)

memory = deque(maxlen=MEMORY)
env = gym.make(ENV_NAME)


def get_action(ob, training=True):
    moves = env.action_space

    if len(moves) == 0:
        return None

    if training and random.uniform(0, 1) < EXPLORE_RATE:
        action = random.choice(moves)
    else:
        action = evaluate(ob, moves)

    return action


def evaluate(observations, moves):
    q_values = []

    observations = np.expand_dims(observations, 0)

    for move in moves:
        action =  np.expand_dims(move, 0)
        inp = [observations, action]
        prediction = model.predict(inp)[0][0]
        # TODO add manual scoring to q value
        q_values.append(prediction)

    if len(q_values) == 0:
        return None

    index = np.argmax(q_values)
    return moves[index]


def get_next_max_q(observations):
    moves = env.action_space
    q_values = []

    observations = np.expand_dims(observations, 0)

    for move in moves:
        action =  np.expand_dims(move, 0)
        inp = [observations, action]
        prediction = model.predict(inp)[0][0]
        q_values.append(prediction)

    if len(q_values) == 0:
        return 0

    return np.max(q_values)


def remember(ob, action, next_ob, reward):
    memory.append((ob, action, next_ob, reward))

def train():
    if SAMPLE_SIZE > len(memory):
        return

    sample_moves = random.sample(memory, SAMPLE_SIZE)
    q_history = []
    loss_history = []
    rewards = 0
    for move in sample_moves:
        ob, action, next_ob, reward = move
        # next q is enemy's best move minus your reward
        next_q = get_next_max_q(next_ob)
        q_value = reward + GAMMA * -next_q
        rewards += reward

        ob = np.expand_dims(ob, 0)
        action =  np.expand_dims(action, 0)
        inp = [ob, action]

        result = model.fit(inp, [q_value], verbose=0)
        q_history.append(q_value)
        loss_history.append(result.history.get('loss', [])[-1])
    print('Max: {:.4f} Min: {:.4f} Mean: {:.4f}, Reward: {:.4f}, loss: {:.4f}'
          .format(np.max(q_history), np.min(q_history), np.mean(q_history), env.rewards, np.mean(loss_history)))


mode = sys.argv[1] if len(sys.argv) > 1 else ''

if mode == 'train':

    while True:

        ob = env.reset()
        a = get_action(ob)
        ob, reward, done, info = env.step(a)
        while True:
            possible_moves = env.action_space
            a = get_action(ob)

            if a is None:
                print("no actions!")
                print(env.past_plays_dracula)
                break  # no more action

            next_ob, next_reward, done, info = env.step(a)

            remember(ob, a, next_ob, reward)

            ob = next_ob
            reward = next_reward

            # if mode != 'silent':
            #     env.render()

            if done:
                break

        # think
        train()
        model.save_weights(WEIGHTS_PATH)

else:
    i = 0
    while True:

        ob = env.reset()
        a = get_action(ob)
        ob, reward, done, info = env.step(a)
        i += 1
        while True:
            possible_moves = env.action_space
            a = get_action(ob)

            if a is None:
                print("no actions!")
                print(env.past_plays_dracula)
                break  # no more action

            next_ob, next_reward, done, info = env.step(a)

            # remember(ob, a, next_ob, reward)

            ob = next_ob
            reward = next_reward

            # if mode != 'silent':
            #     env.render()

            if done:
                break

        print('Game %d - reward %d' % (i, env.rewards))
