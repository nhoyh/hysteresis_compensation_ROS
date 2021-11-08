# hysteresis_compensation_ROS
Young-Hoon Nho (nhoyh1@gmail.com)
collaboration with Dong-Hoon Baek (romansabaek@gmail.com)

This project is published on IEEE Robotics and Automations Letter https://ieeexplore.ieee.org/abstract/document/9591437

"ViO-Com: Feed-forward compensation using vision-based optimization for high-precision surgical manipulation"

Please send me an e-mail if you need "cyclegan.model-42662.data-00000-of-00001" in your checkpoint folder.
(113.2Mb)

Pipeline
-----------
<img width="800" src="https://user-images.githubusercontent.com/42211418/140783567-1bc53378-c129-4cdd-8853-02d77a6e2e92.PNG">

RGB organ image (IRGB) acquired from an endoscopic camera is converted to the binary image by using the modified CycleGAN. The SCNN extracted the features in the binary image and this feature is utilized to search the most similar virtual image to the binary image in the pre-built virtual image dataset (Ivir). During the calibration phase, the sinusoidal periodic signal is given to the surgical manipulator so the sequence data in calibration period (k ∈ 0,1,...,N) including the desired angles (θ(k) desired) and the estimated angles (θˆ(k) ) is obtained simultaneously. With this data, the BFGS optimization method identifies the Bouc-Wen model parameters (non-optimized, P) and the feed-forward compensation using the Bouc-Wen model with optimized parameters (P∗) is finally available to apply to a surgical manipulator.

Surgical tool segmentation results
-----------
<div>
<img width="170" src="https://user-images.githubusercontent.com/42211418/100967344-572c5880-3572-11eb-8613-b21a76f2cd75.png">
<img width="170" src="https://user-images.githubusercontent.com/42211418/100967312-44198880-3572-11eb-81d3-63aa8dab6a1b.png">
<img width="170" src="https://user-images.githubusercontent.com/42211418/100967350-5abfdf80-3572-11eb-9136-7e11f1c98893.png">
<img width="170" src="https://user-images.githubusercontent.com/42211418/100967331-4ed41d80-3572-11eb-9453-2796cc304f22.png">
</div>
