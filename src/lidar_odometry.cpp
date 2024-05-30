// #include "rclcpp/rclcpp.hpp"
// #include "nav_msgs/msg/odometry.hpp"
// #include "tf2_ros/transform_broadcaster.h"
// #include "geometry_msgs/msg/quaternion.hpp"
// #include <tf2/LinearMath/Quaternion.h>

// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <unistd.h>

// struct OdomData
// {
//     double x;
//     double y;
//     double theta;
// };

// class UDPOdometryNode : public rclcpp::Node
// {
// public:
//     UDPOdometryNode() : Node("udp_odometry_node")
//     {
//         odom_pub_ = this->create_publisher<nav_msgs::msg::Odometry>("odom", 10);
//         initialize_udp_socket();
//     }

//     ~UDPOdometryNode()
//     {
//         close(sockfd_);
//     }

// private:
//     void initialize_udp_socket()
//     {
//         sockaddr_in servaddr;
//         // Creating socket file descriptor
//         if ((sockfd_ = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
//         {
//             RCLCPP_FATAL(this->get_logger(), "socket creation failed");
//             exit(EXIT_FAILURE);
//         }

//         memset(&servaddr, 0, sizeof(servaddr));
//         servaddr.sin_family = AF_INET;
//         servaddr.sin_addr.s_addr = INADDR_ANY;
//         servaddr.sin_port = htons(7776);

//         if (bind(sockfd_, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
//         {
//             RCLCPP_FATAL(this->get_logger(), "bind failed");
//             exit(EXIT_FAILURE);
//         }

//         std::thread([this]()
//                     { this->receive_and_publish_odometry(); })
//             .detach();
//     }

//     void receive_and_publish_odometry()
//     {
//         OdomData odom_data;
//         sockaddr_in cliaddr;
//         socklen_t len = sizeof(cliaddr);
//         while (rclcpp::ok())
//         {
//             ssize_t n = recvfrom(sockfd_, &odom_data, sizeof(OdomData), 0, (struct sockaddr *)&cliaddr, &len);
//             if (n > 0)
//             {
//                 publish_odometry(odom_data);
//             }
//         }
//     }

//     void publish_odometry(const OdomData &data)
//     {
//         auto message = nav_msgs::msg::Odometry();
//         message.header.stamp = this->get_clock()->now();
//         message.header.frame_id = "odom";
//         message.child_frame_id = "base_link";

//         // Position
//         message.pose.pose.position.x = data.x;
//         message.pose.pose.position.y = data.y;
//         message.pose.pose.position.z = 0.0;
//         std::cout << "x: " << data.x << std::endl;
//         std::cout << "y: " << data.y << std::endl;
//         std::cout << "theta: " << data.theta << std::endl;

//         // Orientation
//         tf2::Quaternion q;
//         q.setRPY(0, 0, data.theta);
//         message.pose.pose.orientation.x = q.x();
//         message.pose.pose.orientation.y = q.y();
//         message.pose.pose.orientation.z = q.z();
//         message.pose.pose.orientation.w = q.w();

//         odom_pub_->publish(message);
//     }

//     int sockfd_;
//     rclcpp::Publisher<nav_msgs::msg::Odometry>::SharedPtr odom_pub_;
// };

// int main(int argc, char **argv)
// {
//     rclcpp::init(argc, argv);
//     rclcpp::spin(std::make_shared<UDPOdometryNode>());
//     rclcpp::shutdown();
//     return 0;
// }

//################

#include "rclcpp/rclcpp.hpp"
#include "nav_msgs/msg/odometry.hpp"
#include "tf2_ros/transform_broadcaster.h"
#include "geometry_msgs/msg/quaternion.hpp"
#include "geometry_msgs/msg/transform_stamped.hpp"
#include <tf2/LinearMath/Quaternion.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

struct OdomData
{
    double x;
    double y;
    double theta;
};

class UDPOdometryNode : public rclcpp::Node
{
public:
    UDPOdometryNode() : Node("udp_odometry_node")
    {
        odom_pub_ = this->create_publisher<nav_msgs::msg::Odometry>("odom", 100);
        tf_broadcaster_ = std::make_shared<tf2_ros::TransformBroadcaster>(this);
        initialize_udp_socket();
    }

    ~UDPOdometryNode()
    {
        close(sockfd_);
    }

private:
    void initialize_udp_socket()
    {
        sockaddr_in servaddr;
        // Creating socket file descriptor
        if ((sockfd_ = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        {
            RCLCPP_FATAL(this->get_logger(), "socket creation failed");
            exit(EXIT_FAILURE);
        }

        memset(&servaddr, 0, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = INADDR_ANY;
        servaddr.sin_port = htons(7776);

        if (bind(sockfd_, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
        {
            RCLCPP_FATAL(this->get_logger(), "bind failed");
            exit(EXIT_FAILURE);
        }

        std::thread([this]()
                    { this->receive_and_publish_odometry(); })
            .detach();
    }

    void receive_and_publish_odometry()
    {
        OdomData odom_data;
        sockaddr_in cliaddr;
        socklen_t len = sizeof(cliaddr);
        while (rclcpp::ok())
        {
            ssize_t n = recvfrom(sockfd_, &odom_data, sizeof(OdomData), 0, (struct sockaddr *)&cliaddr, &len);
            if (n > 0)
            {
                publish_odometry(odom_data);
            }
        }
    }

    void publish_odometry(const OdomData &data)
    {
        auto message = nav_msgs::msg::Odometry();
        message.header.stamp = this->get_clock()->now();
        message.header.frame_id = "odom";
        message.child_frame_id = "base_link";

        
        message.pose.pose.position.x = data.x;
        message.pose.pose.position.y = data.y;
        message.pose.pose.position.z = 0.0;

        std::cout << "x: " << data.x << std::endl;
        std::cout << "y: " << data.y << std::endl;
        std::cout << "theta: " << data.theta << std::endl;

        
        tf2::Quaternion q;
        q.setRPY(0, 0, data.theta);
        message.pose.pose.orientation.x = q.x();
        message.pose.pose.orientation.y = q.y();
        message.pose.pose.orientation.z = q.z();
        message.pose.pose.orientation.w = q.w();

        odom_pub_->publish(message);

        
        geometry_msgs::msg::TransformStamped transformStamped;
        transformStamped.header.stamp = message.header.stamp;
        transformStamped.header.frame_id = "odom";
        transformStamped.child_frame_id = "base_link";

        transformStamped.transform.translation.x = 0;
        transformStamped.transform.translation.y = 0;
        transformStamped.transform.translation.z = 0.0;
        transformStamped.transform.rotation.x = q.x();
        transformStamped.transform.rotation.y = q.y();
        transformStamped.transform.rotation.z = q.z();
        transformStamped.transform.rotation.w = q.w();
        

        tf_broadcaster_->sendTransform(transformStamped);
    }

    int sockfd_;
    rclcpp::Publisher<nav_msgs::msg::Odometry>::SharedPtr odom_pub_;
    std::shared_ptr<tf2_ros::TransformBroadcaster> tf_broadcaster_;
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<UDPOdometryNode>());
    rclcpp::shutdown();
    return 0;
}
// rm -rf build install log