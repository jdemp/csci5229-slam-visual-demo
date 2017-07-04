# csci5229-slamproject

run make in the directory to compile
then run ./slam_demo

Use arrow keys to navigate around, PageUp & PageDown allow you to move up/down
Press the spacebar to advance through the steps

Pressing 0-9 will take you to the point of view of one of the cameras (with 1
  being the first camera and 0 being the last)
This will also let you see which landmarks were first detected by that camera,
they will be green.

The steps demonstrated in this demo are
1. inserting a new camera
2. detecting features in the camera's frame
3. Showing those features connected to the the camera's center (of the two most recent cameras)
4. Showing the correspondences between the two most recent frames
    This will be shown by creating a point along both projection lines, the point will be blue,
    and connecting the two points (one in each camera frame) by a green line. In actual
    SLAM it is form these that you calculate the transform.
5. From the correspondences calculating the approximate transform between frames
repeat

Frames 6-9 do not have landmarks associated with them, as they are there to demonstrate
loop closure and what happens when a loop is closed. 

With the 10th camera frame, after detecting features (which are the same as the
  first frame despite the initial position being off), will sift to the same position
  as the first frame. This demonstrates loop detection and closure, as the camera
  saw the same landmarks in roughly the same spot as the first frame we can assume
  that the camera should be in roughly the same area. In actual SLAM this results
  in a bundle adjustment happening and changing the position of the landmarks and cameras.
  For the purpose of this demo, I just move the cameras to some slightly different poses.

l : triggers the lighting on and Off
v : allows you to switch between viewing the scene with landmarks, just the scene, or just the landmarks
      the landmark only is what the system would be functionally storing and seeing in spare SLAM.
