#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/point-to-point-dumbbell.h"
#include "ns3/flow-monitor-helper.h"
#include "tutorial-app.h"

namespace offline_3
{
    const uint64_t PACKET_SIZE = 1024;
    const double_t SIMULATION_TIME = 100.0;
}

NS_LOG_COMPONENT_DEFINE("1905039");

static void congestion_window_change(ns3::Ptr<ns3::OutputStreamWrapper> stream, uint32_t oldCwnd, uint32_t newCwnd)
{
    *stream->GetStream() << std::fixed << ns3::Now().GetSeconds() << " " << newCwnd / 1000000.0 << std::endl;
}

int main(int argc, char* argv[])
{
    uint64_t bottleneck_rate = 50;
    double_t loss_rate_exponent = -6;
    std::string algorithm = "TcpWestwoodPlus";
    bool trace_congestion = false;
    bool verbose = false;
    ns3::CommandLine cmd(__FILE__);

    cmd.AddValue("bottleneck-rate", "Set bottlneck data rate", bottleneck_rate);
    cmd.AddValue("loss-rate-exponent", "Set packet loss rate exponent", loss_rate_exponent);
    cmd.AddValue("algorithm", "Set algorithm for second flow", algorithm);
    cmd.AddValue("trace-congestion", "Trace Congestion Window", trace_congestion);
    cmd.AddValue("verbose", "Enable verbose mode", verbose);
    cmd.Parse(argc, argv);

    uint64_t bandwidth_delay_product = (bottleneck_rate * 100000) / offline_3::PACKET_SIZE / 8;

    ns3::Time::SetResolution(ns3::Time::NS);

    ns3::Config::SetDefault("ns3::TcpSocket::SegmentSize", ns3::UintegerValue(offline_3::PACKET_SIZE));

    if(verbose)
    {
        ns3::LogComponentEnable("OnOffApplication", ns3::LOG_LEVEL_INFO);
        ns3::LogComponentEnable("PacketSink", ns3::LOG_LEVEL_INFO);
    }

    ns3::PointToPointHelper sender_p2p_helper;
    ns3::PointToPointHelper reciever_p2p_helper;
    ns3::PointToPointHelper bottleneck_p2p_helper;

    sender_p2p_helper.SetChannelAttribute("Delay", ns3::TimeValue(ns3::Time("1ms")));
    sender_p2p_helper.SetDeviceAttribute("DataRate", ns3::DataRateValue(ns3::DataRate("1Gbps")));
    sender_p2p_helper.SetQueue("ns3::DropTailQueue", "MaxSize", ns3::StringValue(std::to_string(bandwidth_delay_product) + "p"));
    reciever_p2p_helper.SetChannelAttribute("Delay", ns3::TimeValue(ns3::Time("1ms")));
    reciever_p2p_helper.SetDeviceAttribute("DataRate", ns3::DataRateValue(ns3::DataRate("1Gbps")));
    reciever_p2p_helper.SetQueue("ns3::DropTailQueue", "MaxSize", ns3::StringValue(std::to_string(bandwidth_delay_product) + "p"));
    bottleneck_p2p_helper.SetChannelAttribute("Delay", ns3::TimeValue(ns3::Time("100ms")));
    bottleneck_p2p_helper.SetDeviceAttribute("DataRate", ns3::StringValue(std::to_string(bottleneck_rate) + "Mbps"));

    ns3::PointToPointDumbbellHelper p2p_dumbbell_helper(2, sender_p2p_helper, 2, reciever_p2p_helper, bottleneck_p2p_helper);
    // netdevices of bottleneck p2p is set first
    // so we get bottleneck netdevices on index 0
    ns3::Ptr<ns3::NetDevice> left_bottlneck_net_device = p2p_dumbbell_helper.GetLeft()->GetDevice(0);
    ns3::Ptr<ns3::NetDevice> right_bottlneck_net_device = p2p_dumbbell_helper.GetRight()->GetDevice(0);
    ns3::Ptr<ns3::RateErrorModel> left_rate_error_model = ns3::CreateObject<ns3::RateErrorModel>();
    ns3::Ptr<ns3::RateErrorModel> right_rate_error_model = ns3::CreateObject<ns3::RateErrorModel>();

    left_rate_error_model->SetRate(std::pow(10.0, loss_rate_exponent));
    right_rate_error_model->SetRate(std::pow(10.0, loss_rate_exponent));
    left_bottlneck_net_device->SetAttribute("ReceiveErrorModel", ns3::PointerValue(left_rate_error_model));
    right_bottlneck_net_device->SetAttribute("ReceiveErrorModel", ns3::PointerValue(right_rate_error_model));

    ns3::Config::SetDefault("ns3::TcpL4Protocol::SocketType", ns3::StringValue("ns3::TcpNewReno"));

    ns3::InternetStackHelper bottleneck_internet_stack_helper;
    ns3::InternetStackHelper pair_newreno_internet_stack_helper;
    ns3::InternetStackHelper pair_westwood_internet_stack_helper;

    bottleneck_internet_stack_helper.Install({p2p_dumbbell_helper.GetLeft(), p2p_dumbbell_helper.GetRight()});
    bottleneck_internet_stack_helper.Install({p2p_dumbbell_helper.GetLeft(0), p2p_dumbbell_helper.GetRight(0)});
    ns3::Config::SetDefault("ns3::TcpL4Protocol::SocketType", ns3::StringValue("ns3::" + algorithm));
    bottleneck_internet_stack_helper.Install({p2p_dumbbell_helper.GetLeft(1), p2p_dumbbell_helper.GetRight(1)});

    ns3::Ipv4AddressHelper bottleneck_address_helper;
    ns3::Ipv4AddressHelper sender_address_helper;
    ns3::Ipv4AddressHelper reciever_address_helper;

    bottleneck_address_helper.SetBase("10.1.1.0", "255.255.255.0");
    sender_address_helper.SetBase("10.1.2.0", "255.255.255.0");
    reciever_address_helper.SetBase("10.1.4.0", "255.255.255.0");
    p2p_dumbbell_helper.AssignIpv4Addresses(sender_address_helper, reciever_address_helper, bottleneck_address_helper);

    ns3::FlowMonitorHelper flow_monitor_helper;
    ns3::Ptr<ns3::FlowMonitor> flow_monitor = flow_monitor_helper.InstallAll();
    ns3::ApplicationContainer sender_apps;
    ns3::ApplicationContainer reciever_apps;
    ns3::AsciiTraceHelper ascii_trace_helper;

    for(size_t i = 0; i < p2p_dumbbell_helper.LeftCount(); ++i)
    {
        ns3::Ptr<ns3::Socket> socket = ns3::Socket::CreateSocket(p2p_dumbbell_helper.GetLeft(i), ns3::TcpSocketFactory::GetTypeId());
        ns3::Ptr<ns3::TutorialApp> app = ns3::CreateObject<ns3::TutorialApp>();

        if(trace_congestion)
        {
            std::string algorithm_to_lower = algorithm;

            std::transform(algorithm_to_lower.begin(), algorithm_to_lower.end(), algorithm_to_lower.begin(), [](char c)
            {
                return std::tolower(c);
            });

            ns3::Ptr<ns3::OutputStreamWrapper> output_stream_wrapper = ascii_trace_helper.CreateFileStream("out/congestion-window-" + std::to_string(i + 1) + "-" + algorithm_to_lower + ".dat");

            *output_stream_wrapper->GetStream() << "# Time\tWindow" << std::endl;
            
            socket->TraceConnectWithoutContext("CongestionWindow", ns3::MakeBoundCallback(congestion_window_change, output_stream_wrapper));
        }

        app->Setup (socket, ns3::InetSocketAddress(p2p_dumbbell_helper.GetRightIpv4Address(i), 9), offline_3::PACKET_SIZE, UINT32_MAX, ns3::DataRate("1Gbps"));
        p2p_dumbbell_helper.GetLeft (i)->AddApplication(app);
        sender_apps.Add(app);
    }

    for(size_t i = 0; i < p2p_dumbbell_helper.LeftCount(); ++i)
    {
        ns3::PacketSinkHelper packet_sink_helper("ns3::TcpSocketFactory", ns3::InetSocketAddress(ns3::Ipv4Address::GetAny(), 9));

        reciever_apps.Add(packet_sink_helper.Install(p2p_dumbbell_helper.GetRight(i)));
    }

    sender_apps.Start(ns3::Seconds(1));
    reciever_apps.Start(ns3::Seconds(0));
    ns3::Ipv4GlobalRoutingHelper::PopulateRoutingTables();
    ns3::Simulator::Stop(ns3::Seconds(offline_3::SIMULATION_TIME));
    ns3::Simulator::Run();

    const ns3::FlowMonitor::FlowStatsContainer &flow_stats = flow_monitor->GetFlowStats();
    double_t last_rx_time1 = std::max(flow_stats.at(1).timeLastRxPacket.GetSeconds(), flow_stats.at(3).timeLastRxPacket.GetSeconds());
    double_t last_rx_time2 = std::max(flow_stats.at(2).timeLastRxPacket.GetSeconds(), flow_stats.at(4).timeLastRxPacket.GetSeconds());
    double_t throughput1 = ((flow_stats.at(1).rxBytes + flow_stats.at(3).rxBytes) * 8.0) / last_rx_time1;
    double_t throughput2 = ((flow_stats.at(2).rxBytes + flow_stats.at(4).rxBytes) * 8.0) / last_rx_time2;
    uint64_t count = 0;
    uint64_t fairness_numerator = 0;
    uint64_t fairness_denominator = 0;

    for(ns3::FlowMonitor::FlowStatsContainerCI iterator = flow_stats.begin(); iterator != flow_stats.end(); ++iterator)
    {
        fairness_numerator += iterator->second.rxBytes;
        fairness_denominator += iterator->second.rxBytes * iterator->second.rxBytes;
        ++count;
    }

    double_t fairness = (double_t)(fairness_numerator * fairness_numerator) / (count * fairness_denominator);

    std::cout << std::fixed << throughput1 / 1000000.0 << " " << throughput2 / 1000000.0  << " "  << fairness << std::endl;

    ns3::Simulator::Destroy();

    return 0;
}