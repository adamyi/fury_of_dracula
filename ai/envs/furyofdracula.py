from gym import Env, spaces
import numpy as np
import time
import sys
import subprocess
import json
import os


PLAYER_CHAR = ['G', 'S', 'H', 'M', 'D']
SEA_ABBRS = ['AS', 'AO', 'BB', 'BS', 'EC', 'IO', 'IR', 'MS', 'NS', 'TS']
ROOT_DIR = os.path.abspath(os.path.join(os.path.dirname(os.path.realpath(__file__)), os.pardir, os.pardir))

class FuryOfDraculaEnv(Env):
    def __init__(self):
        self.reset()
    def step(self, action):
        self.past_plays_dracula += PLAYER_CHAR[self.player] + action + "...."
        process = subprocess.Popen([os.path.join(ROOT_DIR, "nn_features"), '1'], stdin=subprocess.PIPE, stdout=subprocess.PIPE)
        stdoutdata,_ = process.communicate(input = self.past_plays_dracula)
        result = json.loads(stdoutdata)
        self.past_plays_dracula = self.past_plays_dracula[:-4] + result['move']
        if self.player == 4:
            if action in SEA_ABBRS:
                self.past_plays_hunter += PLAYER_CHAR[self.player] + 'S?' + result['move']
            else:
                self.past_plays_hunter += PLAYER_CHAR[self.player] + 'C?' + result['move']
        else:
            self.past_plays_hunter += PLAYER_CHAR[self.player] + action + result['move']
        for revealed in result['revealed']:
            self.past_plays_hunter = self.past_plays_hunter[:35 * revealed + 28] + self.past_plays_dracula[35 * revealed + 28:35 * (revealed + 1)] + self.past_plays_hunter[35 * (revealed + 1):]

        print("past plays (dracula_view): " + self.past_plays_dracula)
        # print("past plays (hunter view): " + self.past_plays_hunter)

        process = subprocess.Popen([os.path.join(ROOT_DIR, "nn_features"), '0'], stdin=subprocess.PIPE, stdout=subprocess.PIPE)
        if self.player == 4:
            stdoutdata,_ = process.communicate(input = self.past_plays_dracula)
            self.player = 0
        else:
            stdoutdata,_ = process.communicate(input = self.past_plays_hunter)
            self.player += 1

        result = json.loads(stdoutdata)
        self.features = np.asarray(result['features'])

        # print("features: " + json.dumps(self.features))
        #TODO: different for hunter
        reward = 0
        done = False
        if self.features[-2] <= 0:
            reward = -100
            done = True
        elif self.features[-1] <= 0:
            reward = 100
            done = True
        return self.features, reward, done, {}
    def reset(self):
        self.action_space = []
        self.player = 0
        self.past_plays_dracula = ""
        self.past_plays_hunter = ""
        self.features = [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 9, 9, 9, 9, 40, 366]
        return self.features
    def getHunterAIMove(self):
        process = subprocess.Popen(os.path.join(ROOT_DIR, "hunter_ai"), stdin=subprocess.PIPE, stdout=subprocess.PIPE)
        instr =  "{\"past_plays\": \"%s\", \"messages\": []}" % self.past_plays_hunter
        stdoutdata,_ = process.communicate(input = instr)
        result = json.loads(stdoutdata)
        return result['move']
