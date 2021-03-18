clc, clear all, close all;

%% loaddata
method = {'BFGS','CG','COBYLA','NM','Powell','PSO','SLSQP','TNC','GA'};
method_num = size(method,2);
%method info: data type, mean (init cost,	cost,	time), std	(init cost, cost,	time)

for i = 1:method_num
    data_w{i} = load("./"+sprintf(method{i})+".txt");
end


%% success or fail

acc_ = 5;
time_ = 120;
cnt = 0;
case_ = 18;
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
end

%% graph
figure();
subplot(1,3,1);
hold on;
grid on;
bar(mean_cost);
errorbar(1:numel(method),mean_cost,rmse_cost,'.','Color','r'); 
set(gca, 'XTickLabel',method);
set(gca,'XTick',1:numel(method));
xlabel('optimization algorithms','FontSize', 15);
ylabel('cost','FontSize', 15);
hold off;

subplot(1,3,2);
hold on;
grid on;
bar(mean_time);
errorbar(1:numel(method),mean_time,rmse_time,'.','Color','r');    
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
boxplot([success_cost{1},success_cost{2},success_cost{3},success_cost{4},success_cost{5},success_cost{6},success_cost{7},success_cost{8},success_cost{9}]);
set(gca, 'XTickLabel',method);
set(gca,'XTick',1:numel(method));
xlabel('optimization algorithms','FontSize', 15);
ylabel('cost','FontSize', 15);
hold off;

subplot(1,2,2);
hold on;
grid on;
boxplot([success_time{1},success_time{2},success_time{3},success_time{4},success_time{5},success_time{6},success_time{7},success_time{8},success_time{9}]);
set(gca, 'XTickLabel',method);
set(gca,'XTick',1:numel(method));
xlabel('optimization algorithms','FontSize', 15);
ylabel('time','FontSize', 15);
hold off;





