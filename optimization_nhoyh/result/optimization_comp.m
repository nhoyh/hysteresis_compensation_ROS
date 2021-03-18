clc, clear all, close all;

%% loaddata
method = {'BFGS','CG','COBYLA','NM','Powell','PSO','SLSQP','TNC','GA'};
method_num = size(method,2);
%method info: data type, mean (init cost,	cost,	time), std	(init cost, cost,	time)

for i = 1:method_num
    data_w{i} = load("./"+sprintf(method{i})+".txt");
end


%% success or fail
acc_ = 3; % cost 
time_ = 60; % 120 % time cost
cnt = 0;
case_ = 18; % 0 15 30, ... #: 18
for i = 1:method_num
    for j = 1: case_
        if data_w{i}(j,3) < acc_ && data_w{i}(j,4) < time_
            cnt = cnt+1;
            success_cost{i}(j,:) = data_w{i}(j,3);
            success_time{i}(j,:) = data_w{i}(j,4);
        else
            success_cost{i}(j,:) = 0;
            success_time{i}(j,:) = 0;
        end
        succees_rate{i} = cnt/18;
    end
    cnt = 0;
end


%% zero remove
for i = 1:method_num
   final_cost{i} = nonzeros(success_cost{i});
   final_time{i} = nonzeros(success_time{i});
   final_robustness{i} = succees_rate{i};
end
%% if success, then mean & std
for i = 1:method_num
    mean_cost(i,:) = mean(final_cost{i});
    mean_time(i,:) = mean(final_time{i});
    robustness(i,:) = final_robustness{i};
    
    rmse_cost(i,:) = rms(final_cost{i});
    rmse_time(i,:) = rms(final_time{i});
        
    std_cost(i,:) = std(final_cost{i});
    std_time(i,:) = std(final_time{i});
end
robustness = robustness * 100;
%% graph
figure();
subplot(1,3,1);
hold on;
grid on;
bar(mean_cost);
% errorbar(1:numel(method),mean_cost,rmse_cost,'.','Color','r'); 
errorbar(1:numel(method),mean_cost,std_cost,'.','Color','r'); 
set(gca, 'XTickLabel',method);
set(gca,'XTick',1:numel(method));
xlabel('optimization algorithms','FontSize', 15);
ylabel('cost','FontSize', 15);
hold off;

subplot(1,3,2);
hold on;
grid on;
bar(mean_time);
% errorbar(1:numel(method),mean_time,rmse_time,'.','Color','r');    
errorbar(1:numel(method),mean_time,std_time,'.','Color','r');    
set(gca, 'XTickLabel',method);
set(gca,'XTick',1:numel(method));
xlabel('optimization algorithms','FontSize', 15);
ylabel('time','FontSize', 15);
hold off;

subplot(1,3,3);
hold on;
grid on;
bar(robustness);
set(gca, 'XTickLabel',method);
set(gca,'XTick',1:numel(method));
xlabel('optimization algorithms','FontSize', 15);
ylabel('robustness','FontSize', 15);
hold off;

%% IQR
figure();
subplot(1,2,1);
hold on;
grid on;
box_data =[];
box_group = [];
for i = 1 : size(final_cost,2)
    box_data = [box_data ; final_cost{i}];
    box_group = [box_group ; ones(size(final_cost{i},1),1) * (i-1)];
end
boxplot(box_data, box_group);

set(gca, 'XTickLabel',method);
set(gca,'XTick',1:numel(method));
xlabel('optimization algorithms','FontSize', 15);
ylabel('cost','FontSize', 15);
hold off;

subplot(1,2,2);
hold on;
grid on;
box_data =[];
box_group = [];
for i = 1 : size(final_time,2)
    box_data = [box_data ; final_time{i}];
    box_group = [box_group ; ones(size(final_time{i},1),1) * (i-1)];
end
boxplot(box_data, box_group);

set(gca, 'XTickLabel',method);
set(gca,'XTick',1:numel(method));
xlabel('optimization algorithms','FontSize', 15);
ylabel('time','FontSize', 15);
hold off;





