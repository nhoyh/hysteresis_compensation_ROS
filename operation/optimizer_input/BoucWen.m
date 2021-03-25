function h_dot = BoucWen(X,h,theta_d_dot)

h_dot = X(1)*theta_d_dot - X(2)*abs(theta_d_dot).*abs(h).^(X(4)-1).*h - X(3)*theta_d_dot.*abs(h).^(X(4));

end


% Parameter definition
% X(1): A
% X(2): Beta
% X(3): gamma
% X(4): n