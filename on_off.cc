/*

n0-----n1-------n2

*/
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"


using namespace ns3;


NS_LOG_COMPONENT_DEFINE ("OnOff_Example");

int
main (int argc, char *argv[])
{
        CommandLine cmd;
        cmd.Parse (argc, argv);

        NodeContainer nodes,nodes01,nodes12;
        nodes.Create (3); //creating 3 nodes
        nodes01.Add(nodes.Get(0));
	nodes01.Add(nodes.Get(1)); //node container 01 containing node 0 and node 1
	nodes12.Add(nodes.Get(1));
	nodes12.Add(nodes.Get(2));  //node container 12 containing node 1 and node 2

	 
	PointToPointHelper p2pHelper01;  //creating first interface p2pHelper01
	p2pHelper01.SetChannelAttribute ("Delay", StringValue ("1ms"));
	p2pHelper01.SetQueue("ns3::DropTailQueue");
	p2pHelper01.SetDeviceAttribute ("DataRate", StringValue ("200kbps"));
	
	PointToPointHelper p2pHelper12;  //creating second interface p2pHelper12
	p2pHelper12.SetChannelAttribute ("Delay", StringValue ("1ms"));
        p2pHelper12.SetQueue("ns3::DropTailQueue");
	p2pHelper12.SetDeviceAttribute ("DataRate", StringValue ("200kbps"));  

        NetDeviceContainer devices; //installing interfaces on nodes, first inteface installed between node 0 and 1
        devices = p2pHelper01.Install (nodes01);

        NetDeviceContainer devices12;	 //installing interfaces on nodes, second inteface installed between node 1 and 2
	devices12 = p2pHelper12.Install (nodes12);


        InternetStackHelper stack; //creating stack for all nodes
        stack.Install (nodes);

        Ipv4AddressHelper address; //setting IP address for each interface node


	// Defining addresses for nodes 0, 1
        address.SetBase ("10.1.1.0", "255.255.255.0");
        Ipv4InterfaceContainer interfaces = address.Assign (devices);
	
	// Defining addresses for nodes 1, 2
	address.SetBase ("10.1.2.0", "255.255.255.0");
	Ipv4InterfaceContainer interface12 = address.Assign (devices12);

	
        
//------------------------------------------------------------------------------------------------------------------------------------------------
        uint16_t port = 9999; //setting port number

        OnOffHelper onoffHelper("ns3::UdpSocketFactory", InetSocketAddress(interface12.GetAddress(1), port)); //creating socket
	onoffHelper.SetAttribute ("DataRate", StringValue ("10Kbps"));
	onoffHelper.SetAttribute ("PacketSize", UintegerValue(1024));
	onoffHelper.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]")); //send traffic for every 1 sec after stop
	onoffHelper.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=3]")); //stop sending traffic every 3sec after start
	
        ApplicationContainer app = onoffHelper.Install (nodes.Get(0)); //source node create at node 0
    	app.Start (Seconds (0.0)); //start application
    	app.Stop (Seconds (20)); //stop application

    	PacketSinkHelper sink("ns3::UdpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port));   //destination	
        app = sink.Install(nodes12.Get(1)); //destination node created at node 2
    	app.Start (Seconds (0.0)); //start application
    	app.Stop (Seconds (20)); //stop application




//--------------------------------------------------------------------------------------------------------------

// Applying Global routing
        Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

        // Using NetAnim setting nodes co-ordinates
	AnimationInterface::SetConstantPosition (nodes.Get (0), 10.0, 20.0);
	AnimationInterface::SetConstantPosition (nodes.Get (1), 20.0, 20.0);
	AnimationInterface::SetConstantPosition (nodes.Get (2), 30.0, 20.0);
	
	AnimationInterface anim ("OF_anim.xml"); //creating xml file for animation

        AsciiTraceHelper ascii;
        p2pHelper01.EnableAsciiAll (ascii.CreateFileStream ("OnOff.tr")); //generating trace filr
        p2pHelper01.EnablePcapAll ("OnOff"); //generating pcap files


        Simulator::Stop(Seconds(20)); //stop simulation
        Simulator::Run ();
        Simulator::Destroy ();


  return 0;
}
