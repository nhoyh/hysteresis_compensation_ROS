clc, clear all;
close all;

hys_none = load("/home/ldg/Desktop/nobaek/result_data/conf1_2/non/none1.txt");
hys_vision = load("/home/ldg/Desktop/nobaek/result_data/conf1_2/non/vision5.txt");
hys_ours = load("/home/ldg/Desktop/nobaek/result_data/conf1_2/non/ours5.txt");


data_size = 4000;
%4000
%8000
%18000
%time, des_pos, des_vel, 
for i = 1: data_size
    tt_none(i,:)     = hys_none(i,1);
    tt_vision(i,:)   = hys_vision(i,1);
    tt_ours(i,:)     = hys_ours(i,1);
    
    des_pos_none(i,:) = hys_none(i,2);
    des_pos_vision(i,:) = hys_vision(i,2);
    des_pos_ours(i,:) = hys_ours(i,2);
    
    act_pos_none(i,:) = hys_none(i,6);
    act_pos_vision(i,:) = hys_vision(i,6);
    act_pos_ours(i,:) = hys_ours(i,6);
end

figure();
subplot(2,1,1);
hold on;
grid on;
plot(tt_none, des_pos_none, 'k','LineWidth',1);
plot(tt_none, act_pos_none, 'g','LineWidth',1);
plot(tt_vision, act_pos_vision, 'b','LineWidth',1);
plot(tt_ours, act_pos_ours,'r','LineWidth',1);
legend('des pos','none','vision','proposed');

subplot(2,1,2);
hold on;
grid on;
ideal = 0:0.1:60;
plot(ideal, ideal,'k--');
plot(des_pos_none, act_pos_none, 'g','LineWidth',1);
plot(des_pos_vision, act_pos_vision, 'b','LineWidth',1);
plot(des_pos_ours, act_pos_ours,'r','LineWidth',1);
legend('des pos','none','vision','proposed');
xlim([-10, 70]);
ylim([-10, 70]);
%% RMSE
error_none = des_pos_none - act_pos_none;
error_vision = des_pos_vision - act_pos_vision;
error_ours = des_pos_ours - act_pos_ours;

RMSE_none = rms(error_none)
RMSE_vision = rms(error_vision)
RMSE_ours = rms(error_ours)


%% hys case
% clc, clear all;
% close all;
% 
% hys_none_1 = load("/home/ldg/Desktop/nobaek/result_data/conf0_12/60sin2/none1.txt");
% hys_none_2 = load("/home/ldg/Desktop/nobaek/result_data/conf0_17/60sin2/none1.txt");
% hys_none_3 = load("/home/ldg/Desktop/nobaek/result_data/conf1_17/60sin2/none1.txt");
% hys_none_4 = load("/home/ldg/Desktop/nobaek/result_data/conf1_2/60sin2/none2.txt");
% 
% data_size = 4000;
% 
% for i = 1: data_size
%     tt_none_1(i,:)     = hys_none_1(i,1);
%     tt_none_2(i,:)     = hys_none_2(i,1);
%     tt_none_3(i,:)     = hys_none_3(i,1);
%     tt_none_4(i,:)     = hys_none_4(i,1);
%     
%     des_pos_none_1(i,:) = hys_none_1(i,2);
%     des_pos_none_2(i,:) = hys_none_2(i,2);
%     des_pos_none_3(i,:) = hys_none_3(i,2);
%     des_pos_none_4(i,:) = hys_none_4(i,2);
%     
%     act_pos_none_1(i,:) = hys_none_1(i,6);
%     act_pos_none_2(i,:) = hys_none_2(i,6);
%     act_pos_none_3(i,:) = hys_none_3(i,6);
%     act_pos_none_4(i,:) = hys_none_4(i,6);
% end
% 
% figure();
% subplot(2,1,1);
% hold on;
% grid on;
% plot(tt_none_1, des_pos_none_1, 'c','LineWidth',1);
% plot(tt_none_1, act_pos_none_1, 'k','LineWidth',1);
% plot(tt_none_2, act_pos_none_2, 'g','LineWidth',1);
% plot(tt_none_3, act_pos_none_3, 'b','LineWidth',1);
% plot(tt_none_4, act_pos_none_4, 'r','LineWidth',1);
% legend('case1','case2','case3','case4');
% 
% subplot(2,1,2);
% hold on;
% grid on;
% ideal = 0:0.1:60;
% plot(ideal, ideal,'c--');
% plot(des_pos_none_1, act_pos_none_1, 'k','LineWidth',1);
% plot(des_pos_none_2, act_pos_none_2, 'g','LineWidth',1);
% plot(des_pos_none_3, act_pos_none_3, 'b','LineWidth',1);
% plot(des_pos_none_4, act_pos_none_4, 'r','LineWidth',1);
% legend('Ideal','case1','case2','case3','case4');
% xlim([-10, 70]);
% ylim([-10, 70]);

