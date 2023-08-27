#ifndef TUTORIAL_APP_H
#define TUTORIAL_APP_H

#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/point-to-point-dumbbell.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/ipv4-flow-classifier.h"


namespace ns3
{

class Application;

/**
 * Tutorial - a simple Application sending packets.
 */
class TutorialApp : public Application
{
  public:
    TutorialApp();
    ~TutorialApp() override;

    /**
     * Register this type.
     * \return The TypeId.
     */
    static TypeId GetTypeId();

    /**
     * Setup the socket.
     * \param socket The socket.
     * \param address The destination address.
     * \param packetSize The packet size to transmit.
     * \param nPackets The number of packets to transmit.
     * \param dataRate the data rate to use.
     */
    void Setup(Ptr<Socket> socket,
               Address address,
               uint32_t packetSize,
               uint32_t nPackets,
               DataRate dataRate);

  private:
    void StartApplication() override;
    void StopApplication() override;

    /// Schedule a new transmission.
    void ScheduleTx();
    /// Send a packet.
    void SendPacket();

    Ptr<Socket> m_socket;   //!< The transmission socket.
    Address m_peer;         //!< The destination address.
    uint32_t m_packetSize;  //!< The packet size.
    uint32_t m_nPackets;    //!< The number of packets to send.
    DataRate m_dataRate;    //!< The data rate to use.
    EventId m_sendEvent;    //!< Send event.
    bool m_running;         //!< True if the application is running.
    uint32_t m_packetsSent; //!< The number of packets sent.
};

} // namespace ns3

#endif /* TUTORIAL_APP_H */



using namespace ns3;

TutorialApp::TutorialApp()
    : m_socket(nullptr),
      m_peer(),
      m_packetSize(0),
      m_nPackets(0),
      m_dataRate(0),
      m_sendEvent(),
      m_running(false),
      m_packetsSent(0)
{
}

TutorialApp::~TutorialApp()
{
    m_socket = nullptr;
}

/* static */
TypeId
TutorialApp::GetTypeId()
{
    static TypeId tid = TypeId("TutorialApp")
                            .SetParent<Application>()
                            .SetGroupName("Tutorial")
                            .AddConstructor<TutorialApp>();
    return tid;
}

void
TutorialApp::Setup(Ptr<Socket> socket,
                   Address address,
                   uint32_t packetSize,
                   uint32_t nPackets,
                   DataRate dataRate)
{
    m_socket = socket;
    m_peer = address;
    m_packetSize = packetSize;
    m_nPackets = nPackets;
    m_dataRate = dataRate;
}

void
TutorialApp::StartApplication()
{
    m_running = true;
    m_packetsSent = 0;
    m_socket->Bind();
    m_socket->Connect(m_peer);
    SendPacket();
}

void
TutorialApp::StopApplication()
{
    m_running = false;

    if (m_sendEvent.IsRunning())
    {
        Simulator::Cancel(m_sendEvent);
    }

    if (m_socket)
    {
        m_socket->Close();
    }
}

void
TutorialApp::SendPacket()
{
    Ptr<Packet> packet = Create<Packet>(m_packetSize);
    m_socket->Send(packet);

    if (++m_packetsSent < m_nPackets)
    {
        ScheduleTx();
    }
}

void
TutorialApp::ScheduleTx()
{
    if (m_running)
    {
        Time tNext(Seconds(m_packetSize * 8 / static_cast<double>(m_dataRate.GetBitRate())));
        m_sendEvent = Simulator::Schedule(tNext, &TutorialApp::SendPacket, this);
    }
}

//  Topology (point-to-point dumbbell)

//  TCP Senders                     Receivers



AsciiTraceHelper asciiTraceHelper;
std::string plotParam, tcpModel;
std::vector<std::pair<double, uint32_t>> renoTimeVsCwnd, otherTimeVsCwnd;
double thpReno = 0, thpOther = 0, packetLossExp = -6;
uint32_t nRecSendPairs = 2, payloadSize = 1024, bottleneckDataRate = 50;

static void
CwndChangeReno(uint32_t oldCwnd, uint32_t newCwnd)
{
    //NS_LOG_UNCOND(Simulator::Now().GetSeconds() << "\t" << newCwnd);
    renoTimeVsCwnd.push_back({Simulator::Now().GetSeconds(), newCwnd});
}


static void
CwndChangeOther(uint32_t oldCwnd, uint32_t newCwnd)
{
    otherTimeVsCwnd.push_back({Simulator::Now().GetSeconds(), newCwnd});
}


void 
printCongestion() 
{
    Ptr<OutputStreamWrapper> fout1, fout2;
    if (plotParam=="congestion") 
    {
        std::string path1 = "scratch/plots/congestionReno_" + tcpModel + ".dat";
        std::string path2 = "scratch/plots/congestionOther_" + tcpModel + ".dat";
        fout1 = asciiTraceHelper.CreateFileStream(path1, std::ios_base::app);
        fout2 = asciiTraceHelper.CreateFileStream(path2, std::ios_base::app);
        for (uint32_t i=0; i<renoTimeVsCwnd.size(); i++)
        {
            *fout1->GetStream() << renoTimeVsCwnd[i].first << " " << renoTimeVsCwnd[i].second << std::endl; 
        }
        for (uint32_t i=0; i<otherTimeVsCwnd.size(); i++)
        {
            *fout2->GetStream() << otherTimeVsCwnd[i].first << " " << otherTimeVsCwnd[i].second << std::endl; 
        }
    }
    else if (plotParam == "btlneckDataRate") {
        std::string path2 = "scratch/plots/throughput_vs_btlneckDataRate_" + tcpModel + ".dat"; 
        fout1 = asciiTraceHelper.CreateFileStream(path2, std::ios_base::app);
        *fout1->GetStream() << bottleneckDataRate << " " << thpReno << " " << thpOther << std::endl; 
    }
    else if (plotParam == "packetLossExp") {
        std::string path2 = "scratch/plots/throughput_vs_packetLossExp_" + tcpModel + ".dat";
        fout1 = asciiTraceHelper.CreateFileStream(path2, std::ios_base::app);
        *fout1->GetStream() << packetLossExp << " " << thpReno << " " << thpOther << std::endl; 
    }
}


NS_LOG_COMPONENT_DEFINE("Offline3Task1");

int
main(int argc, char* argv[])
{
    CommandLine cmd(__FILE__);
    cmd.AddValue("btlneckDataRate","DataRate of the bottleneck in Mbps", bottleneckDataRate);
    cmd.AddValue("packetLossExp", "Value of n, for 10^-n Packet Loss Rate", packetLossExp);
    cmd.AddValue("plotX", "The parameter to plotted", plotParam);
    cmd.AddValue("tcpModel", "TCP Model to compare against TcpNewReno", tcpModel);
    cmd.Parse(argc, argv);

    Config::SetDefault ("ns3::TcpSocket::SegmentSize", UintegerValue (payloadSize));

    if (plotParam == "congestion") packetLossExp = -6;
    double packetLossRate = std::pow(10.0, packetLossExp);
    std::string bottleneckDataRateStr = std::to_string(bottleneckDataRate) + "Mbps";
    std::string ns3TcpModel = "ns3::" + tcpModel;

    PointToPointHelper dumbbellBottleneck, dumbbellLeft, dumbbellRight;
    dumbbellBottleneck.SetDeviceAttribute("DataRate", StringValue(bottleneckDataRateStr));
    dumbbellBottleneck.SetChannelAttribute("Delay", StringValue("100ms"));
    dumbbellLeft.SetDeviceAttribute("DataRate", StringValue("1Gbps"));
    dumbbellRight.SetDeviceAttribute("DataRate", StringValue("1Gbps"));
    dumbbellLeft.SetChannelAttribute("Delay", StringValue("1ms"));
    dumbbellRight.SetChannelAttribute("Delay", StringValue("1ms"));
    dumbbellLeft.SetQueue("ns3::DropTailQueue", "MaxSize", StringValue (std::to_string (bottleneckDataRate*100 / payloadSize) + "p"));
    dumbbellRight.SetQueue("ns3::DropTailQueue", "MaxSize", StringValue (std::to_string (bottleneckDataRate*100 / payloadSize) + "p"));
    PointToPointDumbbellHelper dumbbellHelper (nRecSendPairs, dumbbellLeft, nRecSendPairs, dumbbellRight, dumbbellBottleneck);



    // NetDeviceContainer devices;
    // devices = pointToPoint.Install(nodes);
    Config::SetDefault ("ns3::TcpL4Protocol::SocketType", StringValue ("ns3::TcpNewReno"));
    InternetStackHelper stack1;
    stack1.Install(dumbbellHelper.GetLeft(0));
    stack1.Install(dumbbellHelper.GetRight(0));
    stack1.Install(dumbbellHelper.GetLeft()), stack1.Install(dumbbellHelper.GetRight());

    Config::SetDefault ("ns3::TcpL4Protocol::SocketType", StringValue (ns3TcpModel));
    InternetStackHelper stack2;
    stack2.Install(dumbbellHelper.GetLeft(1));
    stack2.Install(dumbbellHelper.GetRight(1));

    dumbbellHelper.AssignIpv4Addresses (
        Ipv4AddressHelper("192.23.0.0", "255.255.255.0"), 
        Ipv4AddressHelper("192.25.0.0", "255.255.255.0"),
        Ipv4AddressHelper("192.24.0.0", "255.255.255.0")
    );
    Ipv4GlobalRoutingHelper::PopulateRoutingTables(); 

    std::cout << packetLossRate << std::endl;
    Ptr<RateErrorModel> lossModel = CreateObject<RateErrorModel>();
    lossModel->SetAttribute("ErrorRate", DoubleValue(packetLossRate));
    dumbbellHelper.m_routerDevices.Get(1)->SetAttribute("ReceiveErrorModel", PointerValue(lossModel));
    //dumbbellHelper.m_routerDevices.Get(0)->SetAttribute("ReceiveErrorModel", PointerValue(lossModel));

       // 8. Install FlowMonitor on all nodes
    FlowMonitorHelper flowmon;
    // flowmon.SetMonitorAttribute("MaxPerHopDelay", TimeValue(Seconds(0.3)));
    Ptr<FlowMonitor> monitor = flowmon.InstallAll();

    PacketSinkHelper packetSinkHelper ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny(), 9));
    ApplicationContainer sinkApp = packetSinkHelper.Install({dumbbellHelper.GetRight(0), dumbbellHelper.GetRight(1)});
    sinkApp.Start(Seconds(0.0));
    sinkApp.Stop(Seconds(30.0));
    for (uint32_t i=0; i<nRecSendPairs; i++) 
    {
        Ptr<Socket> ns3TcpSocket = Socket::CreateSocket(dumbbellHelper.GetLeft(i), TcpSocketFactory::GetTypeId());
        if (i==0) ns3TcpSocket->TraceConnectWithoutContext("CongestionWindow", MakeCallback(&CwndChangeReno));
        else if (i==1) ns3TcpSocket->TraceConnectWithoutContext("CongestionWindow", MakeCallback(&CwndChangeOther));
        Ptr<TutorialApp> app = CreateObject<TutorialApp>();
        app->Setup(ns3TcpSocket, InetSocketAddress(dumbbellHelper.GetRightIpv4Address(i), 9), payloadSize, 3000000, DataRate("1Gbps"));
        dumbbellHelper.GetLeft(i)->AddApplication (app);
        app->SetStartTime(Seconds(1.0));
        app->SetStopTime(Seconds(30.0));
    }


    Simulator::Stop(Seconds(31.5));
    Simulator::Run();

    // 10. Print per flow statistics
    // monitor->CheckForLostPackets();
    Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier>(flowmon.GetClassifier());
    FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats(); 
    std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i; uint32_t j;
    for (i=stats.begin(), j=1; i!=stats.end(); ++i, ++j)
    {
        //   Simulator::Stops at "second 10".
        Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow(i->first);
        std::cout << "Flow " << i->first << " (" << t.sourceAddress << " -> " << t.destinationAddress << ")\n";
        double thpKbps = i->second.rxBytes * 8.0 / 29.0 / 1e3;
        if (j%2) thpReno += thpKbps;
        else thpOther += thpKbps;
        std::cout << "  Throughput: " << thpKbps  << " Kbps" << std::endl;
    }


    Simulator::Destroy();

    printCongestion();
    return 0;
    
}