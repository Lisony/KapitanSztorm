import torch
import torch.nn.functional as F
import torch.optim as optim
import numpy as np
from buffer import ReplayBuffer
from nn_test import ActorNetwork, CriticNetwork


device = torch.device("cuda" if torch.cuda.is_available() else "cpu")


class Agent:
    def __init__(self, input_dims, alpha=0.1, beta=0.1, env=None,
                 gamma=0.99, n_actions=2, max_size=1000000, tau=0.005,
                 fc1=400, fc2=300, batch_size=64, noise=0.9):
        self.gamma = gamma
        self.tau = tau
        self.memory = ReplayBuffer(max_size, input_dims, n_actions)
        self.batch_size = batch_size
        self.n_actions = n_actions
        self.noise = noise
        self.max_action = env.action_space.high[0]
        self.min_action = env.action_space.low[0]
        
        input_dim = np.prod(input_dims)
        self.checkpoint_file = "F:\magisterka\model.pt"  # Add this line

        # print(f"Action space: {env.action_space}")
        # print(f"Lower bounds: {env.action_space.low}")
        # print(f"Upper bounds: {env.action_space.high}")
        
        self.actor = ActorNetwork(input_dim = input_dim, fc1_dims=fc1, fc2_dims=fc2, n_actions=n_actions)
        self.critic = CriticNetwork(input_dim = input_dim,action_dim=env.action_space.shape[0],  fc1_dims=fc1, fc2_dims=fc2)
        self.target_actor = ActorNetwork(input_dim = input_dim, fc1_dims=fc1, fc2_dims=fc2, n_actions=n_actions)
        self.target_critic = CriticNetwork(input_dim = input_dim, action_dim=env.action_space.shape[0], fc1_dims=fc1, fc2_dims=fc2)

        self.actor_optimizer = optim.Adam(self.actor.parameters(), lr=alpha)
        self.critic_optimizer = optim.Adam(self.critic.parameters(), lr=beta)

        self.update_network_parameters(tau=1)


    def update_network_parameters(self, tau=None):
            if tau is None:
                tau = self.tau

            for target_param, param in zip(self.target_actor.parameters(), self.actor.parameters()):
                target_param.data.copy_(tau * param.data + (1.0 - tau) * target_param.data)

            for target_param, param in zip(self.target_critic.parameters(), self.critic.parameters()):
                target_param.data.copy_(tau * param.data + (1.0 - tau) * target_param.data)

    def choose_action(self, observation, evaluate=False):
        self.actor.eval()
        state = torch.tensor([observation], dtype=torch.float).to(device)
        mu = self.actor(state).to(device)
        mu_prime = mu
        if not evaluate:
            mu_prime = mu + torch.tensor(self.noise * np.random.normal(size=self.n_actions)).to(device)
        self.actor.train()
        return np.clip(mu_prime.cpu().data.numpy()[0], self.min_action, self.max_action)

    def remember(self, state, action, reward, new_state, done):
        self.memory.store_transition(state, action, reward, new_state, done)

    def save_models(self):
        print('... saving models ...')
        torch.save(self.actor.state_dict(), self.actor.checkpoint_file)
        torch.save(self.target_actor.state_dict(), self.target_actor.checkpoint_file)
        torch.save(self.critic.state_dict(), self.critic.checkpoint_file)
        torch.save(self.target_critic.state_dict(), self.target_critic.checkpoint_file)

    def load_models(self):
        print('... loading models ...')
        self.actor.load_state_dict(torch.load(self.actor.checkpoint_file))
        self.target_actor.load_state_dict(torch.load(self.target_actor.checkpoint_file))
        self.critic.load_state_dict(torch.load(self.critic.checkpoint_file))
        self.target_critic.load_state_dict(torch.load(self.target_critic.checkpoint_file))

    def learn(self):
        if self.memory.mem_cntr < self.batch_size:
            return

        state, action, reward, new_state, done = self.memory.sample_buffer(self.batch_size)

        state = torch.tensor(state, dtype=torch.float).to(device)
        new_state = torch.tensor(new_state, dtype=torch.float).to(device)
        action = torch.tensor(action, dtype=torch.float).to(device)
        reward = torch.tensor(reward, dtype=torch.float).to(device)
        done = torch.tensor(done).to(device)


        reward = reward.view(-1, 1)
        done = done.view(-1, 1)
        # print("State",state)
        # print("Action",action)
        target_actions = self.target_actor(new_state)
        target_critic_value = self.target_critic(new_state, target_actions)
        critic_value = self.critic(state, action)

        # print("New State:", new_state.shape)
        # print("Target action",target_actions.shape)
        # print("target critic",target_critic_value.shape)
    
        target = reward + self.gamma * target_critic_value * (1 - done.float())
       
    
        critic_loss = F.mse_loss(target, critic_value)

        self.critic_optimizer.zero_grad()
        critic_loss.backward()
        self.critic_optimizer.step()

        actor_loss = -self.critic(state, self.actor(state))
        actor_loss = torch.mean(actor_loss)

        self.actor_optimizer.zero_grad()
        actor_loss.backward()
        self.actor_optimizer.step()

        self.update_network_parameters()