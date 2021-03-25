function H = RK_BoucWen(X,h,theta_d_dot,dt)

% 1a chamada
h_dot = BoucWen(X,h,theta_d_dot);
past_h = h;
phi = h_dot;
h = past_h + 0.5*dt*h_dot;

% 2a chamada
h_dot = BoucWen(X,h,theta_d_dot);
phi = phi + 2*h_dot;
h = past_h + 0.5*dt*h_dot;

% 3a chamada
h_dot = BoucWen(X,h,theta_d_dot);
phi = phi + 2*h_dot;
h = past_h + dt*h_dot;

% 4a chamada
h_dot = BoucWen(X,h,theta_d_dot);
H = past_h + (phi+h_dot)*dt/6;
end
