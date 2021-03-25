function J = CostFunction(X,theta_d_dot,dt,theta_d,theta_a)

H = zeros([1 length(theta_d)]);
H(1) = X(1)*theta_d_dot(1);

for i = 2:length(theta_d)
    H(i) = RK_BoucWen(X,H(i-1),theta_d_dot(i),dt);   
end

%  J = ((theta_d - H - theta_a).^2)/length(theta_d);
J = ((X(5)*theta_d - X(6)*H - theta_a).^2)/length(theta_d); % same with T.N.Do's parameter
end
