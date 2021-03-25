clear all
close all
clc

img_1 = imread('imgs//00.png');
img_2 = imread('imgs//60.png');

img_1_gray = rgb2gray(img_1);
img_2_gray = rgb2gray(img_2);
for i = 1: size(img_1,1)
    for j = 1 : size(img_1,2)
        if img_1_gray(i,j) > 15
            img_1_gray(i,j) = 100;
        else
            img_1_gray(i,j) = 0;            
        end
        if img_2_gray(i,j) > 15
            img_2_gray(i,j) = 100;
        else
            img_2_gray(i,j) = 0;            
        end
        
    end
end

guide_1 = zeros(256,256);
guide_2 = zeros(256,256);
for i = 1 : size(img_1_gray,1) -1
    for j = 1 : size(img_1_gray,2) -1
        if abs(int16(img_1_gray(i+1,j)) - int16(img_1_gray(i,j))) == 100 || abs(int16(img_1_gray(i,j+1)) - int16(img_1_gray(i,j)))
            guide_1(i,j) = 100;            
        end
        if abs(int16(img_2_gray(i+1,j)) - int16(img_2_gray(i,j))) == 100 || abs(int16(img_2_gray(i,j+1)) - int16(img_2_gray(i,j)))
            guide_2(i,j) = 100;            
        end
    end
end

rgb_guide = uint8(zeros(256,256,3));

for i = 1 : size(guide_1,1)
    for j = 1 : size(guide_1,2)
        if guide_1(i,j) == 100
            rgb_guide(i,j,2) = 255;
        end
        if guide_2(i,j) == 100
            rgb_guide(i,j,1) = 255;
        end
    end    
end



figure;
subplot(3,2,1)
imshow(img_1_gray);
subplot(3,2,2)
imshow(img_2_gray);
subplot(3,2,3)
imshow(guide_1);
subplot(3,2,4)
imshow(guide_2);
subplot(3,2,5)
imshow(rgb_guide);



imwrite(rgb_guide,'guide4.png');


