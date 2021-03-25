%% Bouc-Wen model based hysteresis modeling (DA ENDO 2019.10.15)
clc;
clear all;
close all;

% hys_data = load("output.txt"); 
hys_data = load("/home/ldg/Desktop/nobaek/result_data/conf1_2/60sin2/none5.txt");
hysteresis_size = size(hys_data,1);

cnt = 1;
for i = 0:4000
    if i>1
        if hys_data(i-1,1) ~= hys_data(i,1) 
            n_hys_data(i,:) = hys_data(i,:);
            if n_hys_data(i,1) ~= 0 %&& rem(i,10)==0
                tt(cnt)      = n_hys_data(i, 1);
                des_pos(cnt) = n_hys_data(i, 2);
                des_vel(cnt) = n_hys_data(i, 3);
                act_pos(cnt) = n_hys_data(i, 5);
                cnt= cnt+1;
            end
        end
    end
end
% figure();
subplot(2,1,1);
hold on;
grid on;
plot(tt, des_pos, 'b');
plot(tt, act_pos, 'r');
% plot(tt, ff_input);
% legend('des pos','aurora','ff input','act av pos');


subplot(2,1,2);
hold on;
ideal = 0:0.1:60;
plot(ideal, ideal,'k--');
plot(des_pos, act_pos, 'r');
legend('Ideal','aurora','estimated pos');
% xlim([-25,100]);
% ylim([-25,100]);

%% Define fitness function
dt = 0.001;
Cost_Func_Boucwen = @(X)sum([CostFunction(X,des_vel,dt, des_pos, act_pos)]);

%% Parameter optimization results by GA
%% 0.txt ,cost 0.72  [0, 0, 0, 0, 0, 0] ~ [1, 0.4, 0.1, 20, 5, 40]
% param = [0.19932552816021953	0.06562212332622118	0.0019143948159761302	9.171007493878557	0.7037128536691256	17.19338898510898];
% param = [ 0.133   4.4064 -0.6567 11.7446  0.703  25.6058];
param = [ 0.3925  0.4624 -0.3181  8.4292  1.0722 14.2741];
A = param(1);
B = param(2);
C = param(3);
n = param(4);
alpha = param(5);
beta = param(6);
% gamma = param(7);

h = zeros(1,size(des_pos,2));
h(1) = A*des_vel(1)*dt;
          
for i = 2:size(des_pos,2)
   h(i) = RK_BoucWen([A B C n],h(i-1),des_vel(i).',dt);
end
 
% RMSE
for i = 1:size(des_pos,2)
    squre_error(i) = (alpha*des_pos(i).' - beta*h(i) - act_pos(i).')^2;
end
RMSE = sqrt(sum(squre_error)/size(des_pos,2));





%% modeling results of pitch
figure()
subplot(2,1,1)
hold on;
grid on;
aux_line = -10:0.1:60;
plot(aux_line, aux_line, 'k--','LineWidth',2);
plot(des_pos, act_pos, 'b-','linewidth',2);
plot(des_pos, alpha*des_pos - beta*h , 'r-','linewidth',2);
legend('ideal','act pos','boucwen');

xlabel('Desired Pos (deg)');
ylabel('Actual Pos (deg)');

grid on;

%%
% figure(2)
subplot(2,1,2)
hold on;
grid on;
plot(tt, des_pos, 'k-','linewidth',2);
plot(tt, act_pos, 'b-','linewidth',2);
plot(tt, (des_pos + beta*h )/ alpha, 'r-','linewidth',2);
legend('desired','actual','compensated input');


