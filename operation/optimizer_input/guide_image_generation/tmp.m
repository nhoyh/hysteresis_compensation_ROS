clear all
close all
clc


fake = imread('060_f.png');
real = imread('060_r.png');

fake = imresize(fake,0.5);

re_fake = uint8(zeros(128,128,3));
re_fake(:,1:70,:) = fake(:,2:71,:);

fake_r = (fake(:,:,1));
fake_g = (fake(:,:,2));
fake_b = (fake(:,:,3));

% 
re_fake_r = (re_fake(:,:,1));
re_fake_g = (re_fake(:,:,2));
re_fake_b = (re_fake(:,:,3));



real_r = (real(:,:,1));
real_g = (real(:,:,2));
real_b = (real(:,:,3));




% fake_r = double(fake(:,:,1));
% fake_g = double(fake(:,:,2));
% fake_b = double(fake(:,:,3));
% 
% 
% real_r = double(real(:,:,1));
% real_g = double(real(:,:,2));
% real_b = double(real(:,:,3));

dif_r = fake_r - real_r;
dif_g = fake_g - real_g;
dif_b = fake_b - real_b;



imwrite(re_fake,'60_fill.png');


