#include <stdio.h>
#include <stdlib.h>

#include <ros/ros.h>
#include <sensor_msgs/fill_image.h>
#include <sensor_msgs/Image.h>

#include "nncam.h"

//Why why why WHY is this not a standard C++ function?
//Comes from http://www.cplusplus.com/forum/beginner/33835/
/*std::string rc(std::string str, const std::string & replace, char ch) {
	// set our locator equal to the first appearance of any character in replace
	size_t i = str.find_first_of(replace);
	int found = 0;
	while (found != std::string::npos) { // While our position in the sting is in range.
		str[found] = ch; // Change the character at position.
		found = str.find_first_of(replace, found+1); // Relocate again.
	}

	return str; // return our new string.
}*/



HNncam hcam;
void *  image_space = NULL;
//std::vector<ros::Publisher> publishers;
ros::Publisher * pub;

static void __stdcall EventCallback(unsigned nEvent, void* pCallbackCtx){
	if (NNCAM_EVENT_IMAGE == nEvent){
		NncamFrameInfoV2 info = { 0 };
		HRESULT hr = Nncam_PullImageV2(hcam, image_space, 24, &info);
		//printf("pull image ok, resolution = %u x %u\n", info.width, info.height);
		
		ros::Time last_update_time = ros::Time::now();
		sensor_msgs::Image i;
		i.header.frame_id = "amscope";
		i.header.stamp.sec = last_update_time.sec;
		i.header.stamp.nsec = last_update_time.nsec;
		fillImage(i, sensor_msgs::image_encodings::RGB8, info.height, info.width, 3 * info.width, reinterpret_cast<const void*>(image_space));
		
		pub->publish(i);
        }
}

int main(int argc, char** argv){

	ros::init(argc, argv, "amscope_driver");
	ros::NodeHandle nh;
	
	hcam = Nncam_Open(NULL);
	if (NULL == hcam){
        	printf("No cameras found. Exiting.\n");
		return 0;
	}
	
	NncamDeviceV2 camera_array[NNCAM_MAX];
	unsigned camera_cnt = Nncam_EnumV2(camera_array);
	
	printf("Detected %u cameras.\n", camera_cnt);
	//publishers = std::vector<ros::Publisher>(camera_cnt);
	
	int w, h;
	HRESULT hr = Nncam_get_Size(hcam, &w, &h);
		
	ros::Publisher p = nh.advertise<sensor_msgs::Image>("/amscope/image_raw", 1, false);
	pub = & p;
	printf("\tPublishing %d x %d.\n", w, h);
	
	image_space = malloc(TDIBWIDTHBYTES(24 * w) * h);
	hr = Nncam_StartPullModeWithCallback(hcam, EventCallback, NULL);
	
	
	
	/*for(int i = 0; i < camera_cnt; i++){
		std::string topic_string =
			"/camera_" +
			rc(std::string(camera_array[i].id), "-", '_') +
			"/image_raw"
		;
		
		int w, h;
		HRESULT hr = Nncam_get_Size(hcam, &w, &h);
		
		publishers[i] = nh.advertise<sensor_msgs::Image>(topic_string, 1, false);
		printf("\tPublishing %s: %d x %d.\n", topic_string.c_str(), w, h);
	}*/
	
    
/*    int nWidth = 0, nHeight = 0;
    HRESULT hr = Nncam_get_Size(hcam, &nWidth, &nHeight);
    if (FAILED(hr))
        printf("failed to get size, hr = %08x\n", hr);
    else
    {
        g_pImageData = malloc(TDIBWIDTHBYTES(24 * nWidth) * nHeight);
        if (NULL == g_pImageData)
            printf("failed to malloc\n");
        else
        {
            hr = Nncam_StartPullModeWithCallback(hcam, EventCallback, NULL);
            if (FAILED(hr))
                printf("failed to start camera, hr = %08x\n", hr);
            else
            {
                printf("press any key to exit\n");
                getc(stdin);
            }
        }
    }*/
    	ros::spin();
    
	Nncam_Close(hcam);
    
	/* cleanup */
	if (image_space){
		free(image_space);
	}
	return 0;
}
