clc, clear all;
close all;

hys_data = load("../output.txt");
% hys_data = load("/home/ldg/Desktop/nobaek/result_data/conf1_2/60sin2/none1.txt");
data_size = size(hys_data,1);
%time, des_pos, des_vel, 
for i = 1: size(hys_data,1)
    tt(i,:)     = hys_data(i,1);
    des_pos(i,:) = hys_data(i,2);
    des_vel(i,:) = hys_data(i,3);
    act_pos(i,:) = hys_data(i,4);
    act_av_pos(i,:) = hys_data(i,5);
    aurora_(i,:) = hys_data(i,6);
%     ff_input(i,:) = hys_data(i,7);
%     la(i,:) = hys_data(i,8);
%     lb(i,:) = hys_data(i,9);
end
figure();
subplot(2,1,1);
hold on;
grid on;
plot(tt, des_pos, 'b','LineWidth',2);
plot(tt, aurora_, 'r','LineWidth',2);
% plot(tt, ff_input);
plot(tt, act_av_pos,'g','LineWidth',2);
legend('des pos','aurora','estimated pos');


subplot(2,1,2);
hold on;
grid on;
ideal = 0:0.1:60;
plot(ideal, ideal,'k--');
plot(des_pos, aurora_, 'r');
plot(des_pos, act_av_pos,'g','LineWidth',1);
legend('Ideal','aurora','estimated pos');
xlim([-25,100]);
ylim([-25,100]);

% figure();
% hold on;
% plot(tt, la);
% plot(tt, lb);

%% RMSE
error = act_av_pos - aurora_;
error_hys = des_pos- aurora_;
RMSE = rms(error)
RMSE_hys = rms(error_hys)
