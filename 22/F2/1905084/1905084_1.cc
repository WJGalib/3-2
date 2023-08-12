#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ssid.h"
#include "ns3/yans-wifi-helper.h"

//                             Network Topology
//
//   Wifi 10.1.1.0                                        Wifi 10.1.3.0 
//                           AP              AP
//  .  .  .   *    *    *    *               *     *    *    *   .  .  .  .
//           |    |    |    |    10.1.2.0     |    |    |    | 
// .  .  .  s2   s1   s0    x --------------  y   r0   r1   r2  .  .  .  .
//                             point-to-point  
//                                   

using namespace ns3;

uint32_t nWifi = 20, nFlow = 10, nPkt = 100, payloadSize = 1024, coverageFactor = 1;
const double TxRange = 5;
uint32_t totalReceived = 0, totalPktRx, totalPktTx = 0;
AsciiTraceHelper asciiTraceHelper;
std::string plotParam;

Ptr<OutputStreamWrapper> foutThp, foutPdr;

NS_LOG_COMPONENT_DEFINE("WifiStatic");

void 
CourseChange(std::string context, Ptr<const MobilityModel> model)
{
    Vector position = model->GetPosition();
    NS_LOG_UNCOND(context << " x = " << position.x << ", y = " << position.y);
}

void
AddToThroughput(Ptr<const Packet> packet, const Address &address) 
{
    totalPktRx++;
    totalReceived += packet->GetSize();
}

void
AddToTxPkt(Ptr<const Packet> packet) 
{
    totalPktTx++;
}

double
getThroughput()
{
    Time now = Simulator::Now();
    return 8.0 * totalReceived/now.GetSeconds() * 1e-6; 
}

double
getPdr()
{
    return (1.0*totalPktRx) / (1.0*totalPktTx); 

}

void
printPlotData()
{
    int param;
    if (plotParam == "") return;
    if (plotParam == "nWifi") param = nWifi;
    else if (plotParam == "nFlow") param = nFlow;
    else if (plotParam == "nPkt") param = nPkt;
    else if (plotParam == "covFac") param = coverageFactor;
    else return;
    NS_LOG_UNCOND(plotParam);
    std::ostringstream thpPath, pdrPath;
    thpPath << "scratch/plots/1/throughput_v_" << plotParam << ".dat";
    pdrPath << "scratch/plots/1/pdr_v_" << plotParam << ".dat";
    foutThp = asciiTraceHelper.CreateFileStream(thpPath.str(), std::ios_base::app);
    foutPdr = asciiTraceHelper.CreateFileStream(pdrPath.str(), std::ios_base::app);
    *foutThp->GetStream() << param << " " << getThroughput() << std::endl;
    *foutPdr->GetStream() << param << " " << getPdr() << std::endl;

}

int
main(int argc, char* argv[])
{
    bool verbose = true, tracing = false;

    CommandLine cmd(__FILE__);
    cmd.AddValue("nWifi", "Number of wifi STA devices", nWifi);
    cmd.AddValue("nFlow", "Number of flows", nFlow);
    cmd.AddValue("nPkt", "Number of packets per second", nPkt);
    cmd.AddValue("covFac", "A multiplier to extend the Tx range", coverageFactor);
    cmd.AddValue("plotX", "The parameter against which outputs will be plotted", plotParam);
    cmd.AddValue("verbose", "Tell echo applications to log if true", verbose);
    cmd.AddValue("tracing", "Enable pcap tracing", tracing);

    cmd.Parse(argc, argv);
    // The underlying restriction of 18 is due to the grid position
    // allocator's configuration; the grid layout will exceed the
    // bounding box if more than 18 nodes are provided.
    // if (nWifi > 18)
    // {
    //     std::cout << "nWifi should be 18 or less; otherwise grid layout exceeds the bounding box"
    //               << std::endl;
    //     return 1;
    // }

    if (verbose)
    {
        LogComponentEnable("OnOffApplication", LOG_LEVEL_INFO);
        LogComponentEnable("PacketSink", LOG_LEVEL_INFO);
    }

    if (nFlow < nWifi/2) nFlow = nWifi/2;

    Config::SetDefault("ns3::TcpSocket::SegmentSize", UintegerValue(payloadSize));

    NodeContainer p2pNodes;
    p2pNodes.Create(2);

    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRate", StringValue("10Mbps"));
    pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));

    NetDeviceContainer p2pDevices;
    p2pDevices = pointToPoint.Install(p2pNodes);

    NodeContainer wifiStaNodes1, wifiStaNodes2;
    wifiStaNodes1.Create(nWifi/2), wifiStaNodes2.Create(nWifi/2);
    NodeContainer wifiApNode1 = p2pNodes.Get(0), wifiApNode2 = p2pNodes.Get(1);

    YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
    channel.AddPropagationLoss("ns3::RangePropagationLossModel", "MaxRange", 
                                ns3::DoubleValue(coverageFactor * TxRange));
    YansWifiPhyHelper phy1, phy2;
    phy1.SetChannel(channel.Create()), phy2.SetChannel(channel.Create());

    WifiMacHelper mac;
    Ssid ssid1 = Ssid("routerX"), ssid2 = Ssid("routerY");

    WifiHelper wifi;

    NetDeviceContainer staDevices1, staDevices2;
    mac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(ssid1), "ActiveProbing", BooleanValue(false));
    staDevices1 = wifi.Install(phy1, mac, wifiStaNodes1);
    mac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(ssid2), "ActiveProbing", BooleanValue(false));
    staDevices2 = wifi.Install(phy2, mac, wifiStaNodes2);

    NetDeviceContainer apDevices1, apDevices2;
    mac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssid1));
    apDevices1 = wifi.Install(phy1, mac, wifiApNode1);
    mac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssid2));
    apDevices2 = wifi.Install(phy2, mac, wifiApNode2);

    MobilityHelper mobility;
    mobility.SetPositionAllocator("ns3::GridPositionAllocator",
                                  "MinX",
                                  DoubleValue(-0.5),
                                  "MinY",
                                  DoubleValue(0.0),
                                  "DeltaX",
                                  DoubleValue(-0.75),
                                  "DeltaY",
                                  DoubleValue(0.75),
                                  "GridWidth",
                                  UintegerValue(nWifi/6),
                                  "LayoutType",
                                  StringValue("RowFirst"));
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(wifiApNode1), mobility.Install(wifiStaNodes1);
    mobility.SetPositionAllocator("ns3::GridPositionAllocator",
                                  "MinX",
                                  DoubleValue(+0.5),
                                  "MinY",
                                  DoubleValue(0.0),
                                  "DeltaX",
                                  DoubleValue(+0.75),
                                  "DeltaY",
                                  DoubleValue(0.75),
                                  "GridWidth",
                                  UintegerValue(nWifi/6),
                                  "LayoutType",
                                  StringValue("RowFirst"));
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(wifiApNode2), mobility.Install(wifiStaNodes2);

    InternetStackHelper stack;
    stack.Install(wifiApNode1), stack.Install(wifiApNode2);
    stack.Install(wifiStaNodes1), stack.Install(wifiStaNodes2);

    Ipv4AddressHelper address;

    address.SetBase("10.1.2.0", "255.255.255.0");
    Ipv4InterfaceContainer p2pInterfaces;
    p2pInterfaces = address.Assign(p2pDevices);

    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer senderInterfaces = address.Assign(staDevices1);
    address.Assign(apDevices1);

    address.SetBase("10.1.3.0", "255.255.255.0");
    Ipv4InterfaceContainer receiverInterfaces = address.Assign(staDevices2);
    address.Assign(apDevices2);

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();


    ApplicationContainer sinkAppContainer;
    for (uint32_t i=0; i<nFlow; i++) 
    {
        PacketSinkHelper sinkHelper("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), 3333+i/(nWifi/2)));
        sinkAppContainer.Add(sinkHelper.Install(wifiStaNodes2.Get(i%(nWifi/2))));
        Ptr<Object> sinkApp = sinkAppContainer.Get(i);
        sinkApp->TraceConnectWithoutContext("Rx", MakeCallback(&AddToThroughput));
    }

    //sink = StaticCast<PacketSink>(sinkApp.Get(0));

    /* Install TCP/UDP Transmitter on the station */
    
    ApplicationContainer senderAppContainer;
    for (uint32_t i=0; i<nFlow; i++) 
    {
        OnOffHelper senderHelper("ns3::TcpSocketFactory", 
                                 InetSocketAddress(receiverInterfaces.GetAddress(i%(nWifi/2)), 3333+i/(nWifi/2)));
        senderHelper.SetAttribute("PacketSize", UintegerValue(payloadSize));
        senderHelper.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
        senderHelper.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
        std::ostringstream onOffDataRate;
        onOffDataRate << (payloadSize * nPkt) << "Bps";
        senderHelper.SetAttribute("DataRate", DataRateValue(DataRate(onOffDataRate.str())));
        senderAppContainer.Add(senderHelper.Install(wifiStaNodes1.Get(i%(nWifi/2))));
        Ptr<Object> senderApp = senderAppContainer.Get(i);
        senderApp->TraceConnectWithoutContext("Tx", MakeCallback(&AddToTxPkt));
    }


    /* Start Applications */
    senderAppContainer.Start(Seconds(1.0));
    sinkAppContainer.Start(Seconds(0.0));

    Simulator::Stop(Seconds(10.1));
// 
    // if (tracing)
    // {
    //     phy1.SetPcapDataLinkType(WifiPhyHelper::DLT_IEEE802_11_RADIO);
    //     pointToPoint.EnablePcapAll("f2-static");
    //     phy1.EnablePcap("f2-static", apDevices1.Get(0));
    // }

    for (uint32_t k=0; k<nWifi/2; k++) 
    {
        std::ostringstream oss1, oss2;
        oss1 << "/NodeList/" << wifiStaNodes1.Get(k)->GetId() << "/$ns3::MobilityModel/CourseChange";
        Config::Connect(oss1.str(), MakeCallback(&CourseChange));
        oss2 << "/NodeList/" << wifiStaNodes2.Get(k)->GetId() << "/$ns3::MobilityModel/CourseChange";
        Config::Connect(oss2.str(), MakeCallback(&CourseChange));
    }

    Simulator::Schedule(Seconds(10.0), &printPlotData);

    Simulator::Run();
    Simulator::Destroy();
    
    return 0;
}
