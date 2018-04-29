
#include "processingthread.h"

#include <QDebug>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "global.h"

DEFINE_int32(logging_level,             3,              "The logging level. Integer in the range [0, 255]. 0 will output any log() message, while"
" 255 will not output any. Current OpenPose library messages are in the range 0-4: 1 for"
" low priority messages and 4 for important ones.");
DEFINE_bool(disable_multi_thread,       false,          "It would slightly reduce the frame rate in order to highly reduce the lag. Mainly useful"
" for 1) Cases where it is needed a low latency (e.g. webcam in real-time scenarios with"
" low-range GPU devices); and 2) Debugging OpenPose when it is crashing to locate the"
" error.");
DEFINE_int32(profile_speed,             1000,           "If PROFILER_ENABLED was set in CMake or Makefile.config files, OpenPose will show some"
" runtime statistics at this frame number.");
// Producer
DEFINE_string(image_dir,                "examples/media/",      "Process a directory of images. Read all standard formats (jpg, png, bmp, etc.).");
DEFINE_double(camera_fps,               30.0,           "Frame rate for the webcam (also used when saving video). Set this value to the minimum"
" value between the OpenPose displayed speed and the webcam real frame rate.");
// OpenPose
DEFINE_string(model_folder,             "/media/zjg/workspace/Qt-demo/video/models/",      "Folder path (absolute or relative) where the models (pose, face, ...) are located.");
DEFINE_string(output_resolution,        "-1x-1",        "The image resolution (display and output). Use \"-1x-1\" to force the program to use the"
" input image resolution.");
DEFINE_int32(num_gpu,                   -1,             "The number of GPU devices to use. If negative, it will use all the available GPUs in your"
" machine.");
DEFINE_int32(num_gpu_start,             0,              "GPU device start number.");
DEFINE_int32(keypoint_scale,            0,              "Scaling of the (x,y) coordinates of the final pose data array, i.e. the scale of the (x,y)"
" coordinates that will be saved with the `write_keypoint` & `write_keypoint_json` flags."
" Select `0` to scale it to the original source resolution, `1`to scale it to the net output"
" size (set with `net_resolution`), `2` to scale it to the final output size (set with"
" `resolution`), `3` to scale it in the range [0,1], and 4 for range [-1,1]. Non related"
" with `scale_number` and `scale_gap`.");
DEFINE_int32(number_people_max,         -1,             "This parameter will limit the maximum number of people detected, by keeping the people with"
" top scores. The score is based in person area over the image, body part score, as well as"
" joint score (between each pair of connected body parts). Useful if you know the exact"
" number of people in the scene, so it can remove false positives (if all the people have"
" been detected. However, it might also include false negatives by removing very small or"
" highly occluded people. -1 will keep them all.");
// OpenPose Body Pose
DEFINE_bool(body_disable,               false,          "Disable body keypoint detection. Option only possible for faster (but less accurate) face"
" keypoint detection.");
DEFINE_string(model_pose,               "COCO",         "Model to be used. E.g. `COCO` (18 keypoints), `MPI` (15 keypoints, ~10% faster), "
"`MPI_4_layers` (15 keypoints, even faster but less accurate).");
DEFINE_string(net_resolution,           "-1x368",       "Multiples of 16. If it is increased, the accuracy potentially increases. If it is"
" decreased, the speed increases. For maximum speed-accuracy balance, it should keep the"
" closest aspect ratio possible to the images or videos to be processed. Using `-1` in"
" any of the dimensions, OP will choose the optimal aspect ratio depending on the user's"
" input value. E.g. the default `-1x368` is equivalent to `656x368` in 16:9 resolutions,"
" e.g. full HD (1980x1080) and HD (1280x720) resolutions.");
DEFINE_int32(scale_number,              1,              "Number of scales to average.");
DEFINE_double(scale_gap,                0.3,            "Scale gap between scales. No effect unless scale_number > 1. Initial scale is always 1."
" If you want to change the initial scale, you actually want to multiply the"
" `net_resolution` by your desired initial scale.");
// OpenPose Body Pose Heatmaps and Part Candidates
DEFINE_bool(heatmaps_add_parts,         false,          "If true, it will fill op::Datum::poseHeatMaps array with the body part heatmaps, and"
" analogously face & hand heatmaps to op::Datum::faceHeatMaps & op::Datum::handHeatMaps."
" If more than one `add_heatmaps_X` flag is enabled, it will place then in sequential"
" memory order: body parts + bkg + PAFs. It will follow the order on"
" POSE_BODY_PART_MAPPING in `src/openpose/pose/poseParameters.cpp`. Program speed will"
" considerably decrease. Not required for OpenPose, enable it only if you intend to"
" explicitly use this information later.");
DEFINE_bool(heatmaps_add_bkg,           false,          "Same functionality as `add_heatmaps_parts`, but adding the heatmap corresponding to"
" background.");
DEFINE_bool(heatmaps_add_PAFs,          false,          "Same functionality as `add_heatmaps_parts`, but adding the PAFs.");
DEFINE_int32(heatmaps_scale,            2,              "Set 0 to scale op::Datum::poseHeatMaps in the range [-1,1], 1 for [0,1]; 2 for integer"
" rounded [0,255]; and 3 for no scaling.");
DEFINE_bool(part_candidates,            false,          "Also enable `write_json` in order to save this information. If true, it will fill the"
" op::Datum::poseCandidates array with the body part candidates. Candidates refer to all"
" the detected body parts, before being assembled into people. Note that the number of"
" candidates is equal or higher than the number of final body parts (i.e. after being"
" assembled into people). The empty body parts are filled with 0s. Program speed will"
" slightly decrease. Not required for OpenPose, enable it only if you intend to explicitly"
" use this information.");
// OpenPose Face
DEFINE_bool(face,                       false,          "Enables face keypoint detection. It will share some parameters from the body pose, e.g."
" `model_folder`. Note that this will considerable slow down the performance and increse"
" the required GPU memory. In addition, the greater number of people on the image, the"
" slower OpenPose will be.");
DEFINE_string(face_net_resolution,      "368x368",      "Multiples of 16 and squared. Analogous to `net_resolution` but applied to the face keypoint"
" detector. 320x320 usually works fine while giving a substantial speed up when multiple"
" faces on the image.");
// OpenPose Hand
DEFINE_bool(hand,                       false,          "Enables hand keypoint detection. It will share some parameters from the body pose, e.g."
" `model_folder`. Analogously to `--face`, it will also slow down the performance, increase"
" the required GPU memory and its speed depends on the number of people.");
DEFINE_string(hand_net_resolution,      "368x368",      "Multiples of 16 and squared. Analogous to `net_resolution` but applied to the hand keypoint"
" detector.");
DEFINE_int32(hand_scale_number,         1,              "Analogous to `scale_number` but applied to the hand keypoint detector. Our best results"
" were found with `hand_scale_number` = 6 and `hand_scale_range` = 0.4.");
DEFINE_double(hand_scale_range,         0.4,            "Analogous purpose than `scale_gap` but applied to the hand keypoint detector. Total range"
" between smallest and biggest scale. The scales will be centered in ratio 1. E.g. if"
" scaleRange = 0.4 and scalesNumber = 2, then there will be 2 scales, 0.8 and 1.2.");
DEFINE_bool(hand_tracking,              false,          "Adding hand tracking might improve hand keypoints detection for webcam (if the frame rate"
" is high enough, i.e. >7 FPS per GPU) and video. This is not person ID tracking, it"
" simply looks for hands in positions at which hands were located in previous frames, but"
" it does not guarantee the same person ID among frames.");
// OpenPose 3-D Reconstruction
DEFINE_bool(3d,                         false,          "Running OpenPose 3-D reconstruction demo: 1) Reading from a stereo camera system."
" 2) Performing 3-D reconstruction from the multiple views. 3) Displaying 3-D reconstruction"
" results. Note that it will only display 1 person. If multiple people is present, it will"
" fail.");
DEFINE_int32(3d_min_views,              -1,             "Minimum number of views required to reconstruct each keypoint. By default (-1), it will"
" require all the cameras to see the keypoint in order to reconstruct it.");
DEFINE_int32(3d_views,                  1,              "Complementary option to `--image_dir` or `--video`. OpenPose will read as many images per"
" iteration, allowing tasks such as stereo camera processing (`--3d`). Note that"
" `--camera_parameters_folder` must be set. OpenPose must find as many `xml` files in the"
" parameter folder as this number indicates.");
// OpenPose Rendering
DEFINE_int32(part_to_show,              0,              "Prediction channel to visualize (default: 0). 0 for all the body parts, 1-18 for each body"
" part heat map, 19 for the background heat map, 20 for all the body part heat maps"
" together, 21 for all the PAFs, 22-40 for each body part pair PAF.");
DEFINE_bool(disable_blending,           false,          "If enabled, it will render the results (keypoint skeletons or heatmaps) on a black"
" background, instead of being rendered into the original image. Related: `part_to_show`,"
" `alpha_pose`, and `alpha_pose`.");
// OpenPose Rendering Pose
DEFINE_double(render_threshold,         0.05,           "Only estimated keypoints whose score confidences are higher than this threshold will be"
" rendered. Generally, a high threshold (> 0.5) will only render very clear body parts;"
" while small thresholds (~0.1) will also output guessed and occluded keypoints, but also"
" more false positives (i.e. wrong detections).");
DEFINE_int32(render_pose,               -1,             "Set to 0 for no rendering, 1 for CPU rendering (slightly faster), and 2 for GPU rendering"
" (slower but greater functionality, e.g. `alpha_X` flags). If -1, it will pick CPU if"
" CPU_ONLY is enabled, or GPU if CUDA is enabled. If rendering is enabled, it will render"
" both `outputData` and `cvOutputData` with the original image and desired body part to be"
" shown (i.e. keypoints, heat maps or PAFs).");
DEFINE_double(alpha_pose,               0.6,            "Blending factor (range 0-1) for the body part rendering. 1 will show it completely, 0 will"
" hide it. Only valid for GPU rendering.");
DEFINE_double(alpha_heatmap,            0.7,            "Blending factor (range 0-1) between heatmap and original frame. 1 will only show the"
" heatmap, 0 will only show the frame. Only valid for GPU rendering.");
// OpenPose Rendering Face
DEFINE_double(face_render_threshold,    0.4,            "Analogous to `render_threshold`, but applied to the face keypoints.");
DEFINE_int32(face_render,               -1,             "Analogous to `render_pose` but applied to the face. Extra option: -1 to use the same"
" configuration that `render_pose` is using.");
DEFINE_double(face_alpha_pose,          0.6,            "Analogous to `alpha_pose` but applied to face.");
DEFINE_double(face_alpha_heatmap,       0.7,            "Analogous to `alpha_heatmap` but applied to face.");
// OpenPose Rendering Hand
DEFINE_double(hand_render_threshold,    0.2,            "Analogous to `render_threshold`, but applied to the hand keypoints.");
DEFINE_int32(hand_render,               -1,             "Analogous to `render_pose` but applied to the hand. Extra option: -1 to use the same"
" configuration that `render_pose` is using.");
DEFINE_double(hand_alpha_pose,          0.6,            "Analogous to `alpha_pose` but applied to hand.");
DEFINE_double(hand_alpha_heatmap,       0.7,            "Analogous to `alpha_heatmap` but applied to hand.");
// Result Saving
DEFINE_string(write_images,             "",             "Directory to write rendered frames in `write_images_format` image format.");
DEFINE_string(write_images_format,      "png",          "File extension and format for `write_images`, e.g. png, jpg or bmp. Check the OpenCV"
" function cv::imwrite for all compatible extensions.");
DEFINE_string(write_video,              "",             "Full file path to write rendered frames in motion JPEG video format. It might fail if the"
" final path does not finish in `.avi`. It internally uses cv::VideoWriter.");
DEFINE_string(write_json,               "",             "Directory to write OpenPose output in JSON format. It includes body, hand, and face pose"
" keypoints (2-D and 3-D), as well as pose candidates (if `--part_candidates` enabled).");
DEFINE_string(write_coco_json,          "",             "Full file path to write people pose data with JSON COCO validation format.");
DEFINE_string(write_heatmaps,           "",             "Directory to write body pose heatmaps in PNG format. At least 1 `add_heatmaps_X` flag"
" must be enabled.");
DEFINE_string(write_heatmaps_format,    "png",          "File extension and format for `write_heatmaps`, analogous to `write_images_format`."
" For lossless compression, recommended `png` for integer `heatmaps_scale` and `float` for"
" floating values.");
DEFINE_string(write_keypoint,           "",             "(Deprecated, use `write_json`) Directory to write the people pose keypoint data. Set format"
" with `write_keypoint_format`.");
DEFINE_string(write_keypoint_format,    "yml",          "(Deprecated, use `write_json`) File extension and format for `write_keypoint`: json, xml,"
" yaml & yml. Json not available for OpenCV < 3.0, use `write_keypoint_json` instead.");
DEFINE_string(write_keypoint_json,      "",             "(Deprecated, use `write_json`) Directory to write people pose data in JSON format,"
" compatible with any OpenCV version.");

// Configuration header file
//构造函数，指定缓冲区，和图片宽度、高度
ProcessingThread::ProcessingThread():QThread()
{
    // Initialize variables
    stopped1=false;
    qRegisterMetaType<vector<pair<float,float>>>("vector<pair<float,float>>");
} // ProcessingThread constructor

ProcessingThread::~ProcessingThread()
{
} // ProcessingThread destructor

//处理线程的主功能函数


//停止处理线程
void ProcessingThread::stopProcessingThread()
{
    stoppedMutex.lock();
    stopped1=true;
    stoppedMutex.unlock();
} // stopProcessingThread()



void ProcessingThread::run()
{

    op::log("OpenPose Library Tutorial - Example 1.", op::Priority::High);
    // ------------------------- INITIALIZATION -------------------------
    // Step 1 - Set logging level
    // - 0 will output all the logging messages
    // - 255 will output nothing
    op::check(0 <= FLAGS_logging_level && FLAGS_logging_level <= 255, "Wrong logging_level value.",
              __LINE__, __FUNCTION__, __FILE__);
    op::ConfigureLog::setPriorityThreshold((op::Priority)FLAGS_logging_level);
    op::log("", op::Priority::Low, __LINE__, __FUNCTION__, __FILE__);
    // Step 2 - Read Google flags (user defined configuration)
    // outputSize
    const auto outputSize = op::flagsToPoint(FLAGS_output_resolution, "-1x-1");
    // netInputSize
    const auto netInputSize = op::flagsToPoint(FLAGS_net_resolution, "-1x368");
    // poseModel
    const auto poseModel = op::flagsToPoseModel(FLAGS_model_pose);
    // Check no contradictory flags enabled
    if (FLAGS_alpha_pose < 0. || FLAGS_alpha_pose > 1.)
        op::error("Alpha value for blending must be in the range [0,1].", __LINE__, __FUNCTION__, __FILE__);
    if (FLAGS_scale_gap <= 0. && FLAGS_scale_number > 1)
        op::error("Incompatible flag configuration: scale_gap must be greater than 0 or scale_number = 1.",
                  __LINE__, __FUNCTION__, __FILE__);
    // Enabling Google Logging
    const bool enableGoogleLogging = true;
    // Logging
    op::log("", op::Priority::Low, __LINE__, __FUNCTION__, __FILE__);
    // Step 3 - Initialize all required classes
    op::ScaleAndSizeExtractor scaleAndSizeExtractor(netInputSize, outputSize, FLAGS_scale_number, FLAGS_scale_gap);
    op::CvMatToOpInput cvMatToOpInput;
    op::CvMatToOpOutput cvMatToOpOutput;
    op::PoseExtractorCaffe poseExtractorCaffe{poseModel, FLAGS_model_folder,
                                              FLAGS_num_gpu_start, {}, op::ScaleMode::ZeroToOne, enableGoogleLogging};
    op::PoseCpuRenderer poseRenderer{poseModel, (float)FLAGS_render_threshold, !FLAGS_disable_blending,
                                     (float)FLAGS_alpha_pose};
    op::OpOutputToCvMat opOutputToCvMat;
    op::FrameDisplayer frameDisplayer{"OpenPose Tutorial - Example 1", outputSize};
    // Step 4 - Initialize resources on desired thread (in this case single thread, i.e. we init resources here)
    poseExtractorCaffe.initializationOnThread();
    poseRenderer.initializationOnThread();
    //cv::Mat inputImage = cv::imread("/home/neu-lu/openpose/examples/media/COCO_val2014_000000000257.jpg");
    //cv::imshow("test",inputImage);



    cv::Mat frame;
    while(cap.isOpened())
    {
        /////////////////////////////////
        // Stop thread if stopped=TRUE //测试是否停止的过程要加锁
        /////////////////////////////////
        stoppedMutex1.lock();
        if (stopped1)
        {
            stopped1=false;
            stoppedMutex1.unlock();
            break;
        }
        stoppedMutex1.unlock();
        /////////////////////////////////

        cap>>frame;
        cap>>frame;
        cap>>frame;
        updateMembersMutex.lock();

        cv::Mat inputImage = frame;
        if(inputImage.empty())
            op::error("Could not open or find the image: ");
        //op::error("Could not open or find the image: " + FLAGS_image_path, __LINE__, __FUNCTION__, __FILE__);
        const op::Point<int> imageSize{inputImage.cols, inputImage.rows};
        // Step 2 - Get desired scale sizes
        std::vector<double> scaleInputToNetInputs;
        std::vector<op::Point<int>> netInputSizes;
        double scaleInputToOutput;
        op::Point<int> outputResolution;
        std::tie(scaleInputToNetInputs, netInputSizes, scaleInputToOutput, outputResolution)
                = scaleAndSizeExtractor.extract(imageSize);
        // Step 3 - Format input image to OpenPose input and output formats
        const auto netInputArray = cvMatToOpInput.createArray(inputImage, scaleInputToNetInputs, netInputSizes);
        auto outputArray = cvMatToOpOutput.createArray(inputImage, scaleInputToOutput, outputResolution);
        // Step 4 - Estimate poseKeypoints
        poseExtractorCaffe.forwardPass(netInputArray, imageSize, scaleInputToNetInputs);
        const auto poseKeypoints = poseExtractorCaffe.getPoseKeypoints();
        // Step 5 - Render poseKeypoints
        poseRenderer.renderPose(outputArray, poseKeypoints, scaleInputToOutput);
        // Step 6 - OpenPose output format to cv::Mat
        auto outputImage = opOutputToCvMat.formatToCvMat(outputArray);



        //------------------------------处理每一帧的关键点---------------------------------
        const auto numberPeopleDetected = poseKeypoints.getSize(0);
        const auto numberBodyParts = poseKeypoints.getSize(1);
        // Easy version
        int person=0;
        if(numberPeopleDetected>1)person=1;
        vector<pair<float,float> > points;
        points.clear();
        for(int i=0;i<=16;i++)
        {
            float x = poseKeypoints[{person, i, 0}];
            float y = poseKeypoints[{person, i, 1}];
            points.push_back(make_pair(x,y));
        }


        // ------------------------- SHOWING RESULT AND CLOSING -------------------------
        // Step 1 - Show results
        //frameDisplayer.displayFrame(outputImage, 0);
        //cv::imshow("test",outputImage);// + cv::waitKey(0)
        // Step 2 - Logging information message

        // Convert Mat to QImage: Show BGR frame
        QImage img= QImage((const unsigned char*)(outputImage.data),outputImage.cols,outputImage.rows,QImage::Format_RGB888);
        img=img.scaled(500,250);
        updateMembersMutex.unlock();

        // Inform GUI thread of new frame (QImage)
        //发出信号，通知GUI线程有新处理好的一帧
        emit newFrame(img);
        emit newPoint(points);
        //ui->label_player->setPixmap(QPixmap::fromImage(img));

        //cv::waitKey(1);
        //qApp->processEvents();
    }
}
