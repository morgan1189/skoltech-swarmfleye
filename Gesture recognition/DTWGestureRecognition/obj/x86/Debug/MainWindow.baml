   M S B A M L   `   `   `  ����  DTWGestureRecognition&     DTWGestureRecognition.MainWindow   5   	   \ XPresentationFramework, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35R NWindowsBase, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35W SPresentationCore, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35D 9http://schemas.microsoft.com/winfx/2006/xaml/presentation    5   	   R NSystem.Xaml, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089:x,http://schemas.microsoft.com/winfx/2006/xaml     5   	   -   5          � Title$  	KinectDTW��5   	   $	��716��6   $	��640��6(   .��5      �  � RowDefinitions 5   
   �� 5      $��80 �6   �� 5      $	��140 �6   �� 5	      $	��480 �6   �� 5
      $��16 �6    � ColumnDefinitions 5   
   �� 5      $	��200 �6   �� 5      $	��440 �6   .+�5   
   �� $��0��6   $��0��6%   $��2��62   .+�5      �� $3��-/DTWGestureRecognition;component/HeaderBG.png��6   �� 5      $��40��6   $?��9Kinect SDK Dynamic Time Warping (DTW) Gesture Recognition��6    $��14(�6d   $
��Bold"�6r   $��Center=�6�   $��Center=�6�   $	��600��6�   $��SemiExpanded&�6�   $��Normal$�6�   $��Center=�6�   $	��Top=�6  $��Segoe UI)�69  6Q  5      �� 5   
   $��0��6   $��1��6%   .+�5      �� -   5        $��dtwRead��5      $
��Read��6$   $	��200��6H   {�.  6T   �� 5      -   5        $��
dtwCapture��5      $��Capture��6'   $
��200 ��6Q   �� 5      -   5        $��dtwStore��5      $��Store��6%   {�.  $	��200��6]   �� 5      -   5        $��dtwLoadFile��5      $��Load gesture file��6(   {�. $	��200��6i   �� 5      -   5        $��dtwSaveToFile��5      $��Save to file��6*   {�. $	��200��6h   �� 5      -   5        $��dtwShowGestureTest��5      $��Show Gesture Text��6/   {�. $	��200��6w   5      �� 5   
   $��1��6   $��1��6%   $
��Left=�62   .+�5      �� -   5        $��gestureList��5      $	��400��6*   .@�5      ��  �� 
IsSelected . .��61   @Right hand swipe left6I   �� 5      .��6   @Right hand swipe right68   �� 5       .��6   @Left hand swipe left66   �� 5!      .��6   @Left hand swipe right67   �� 5"      .��6   @Two hands zoom in63   �� 5#      .��6   @Two hands zoom out64   �� 5$      .��6   @Right hand wave61   �� 5%      .��6   @Left hand wave60   �� 5&      .��6   @Right hand pull down66   �� 5'      .��6   @Left hand pull down65   �� 5(      .��6   @Right hand push up64   �� 5)      .��6   @Left hand push up63   �� 5*      .��6   @Both hands pull down66   �� 5+      .��6   @Both hands push up64   �� 5,      .��6   @Get on down at the disco!6;   5-      �� 5.      -	   5        $��results��5.   $   $��14(�6   $��0,0,0,0q�63   |�.  $��0q�6V   $�� ��6j    �� TextAlignment$
 Left=�6r   $
��Left=�6�   $	��Top=�6�   $	��400��6�   $��20��6�   �� 5/      -
   5        $��status��5/   $   $��14(�6   $��0,0,0,0q�62   |�.  $��0q�6U   $�� ��6i   $
 Left=�6q   $
��Left=�6�   $	��Top=�6�   $	��400��6�   $��20��6�   �� 50      -   5        $��currentBufferFrame��50   $   $��14(�6   $��0,0,0,0q�6>   |�.  $��0q�6a   $�� ��6u   $
 Left=�6}   $
��Left=�6�   $	��Top=�6�   $	��400��6�   $��20��6�   �� 51      -   5        $��	frameRate��51   $   $��14(�6   $��0,0,0,0q�65   |�.  $��0q�6X   $�� ��6l   $
 Left=�6t   $
��Left=�6�   $	��Top=�6�   $	��400��6�   $��20��6�   �� 52      -   5        $��kinectStatus��52   $   $��14(�6   $��0,0,0,0q�68   |�.  $��0q�6[   $�� ��6o   $
 Left=�6w   $
��Left=�6�   $	��Top=�6�   $	��400��6�   $��20��6�   53      (� 54   
    (� Stretch$ Uniform=�6   $��2��6$   $��2��61   .��55      � .+�56      �� -   5        $��
videoImage��56   1   $	��640��6   $	��480��6$   61   �� 57      -   5        $��skeletonCanvas��57      
������ 6K   58      59      �� 5;   
   $��0��6   $��3��6    $��2��6-   $3��-By Rhemyst and Rymix - kinectdtw.codeplex.com��6A   
�������$	��Top=�6�   $��Center=�6�   $��16��6�   $��10(�6�   $��0q�6�   ��5<      �� 5=       �� EndPoint$ 0.5,1&�6&    �� 
StartPoint$ 0.5,0&�67   .��5>      �  � Color$ 	#FF4D35AB��6#   	 � Offset$	 1Q�65   � 5?      $ 	#FF724798��6#   $	 0.498Q�65   � 5@      $ 	#FF2B9E32��6#   $	 0Q�65   5A      5C      5D      5E      