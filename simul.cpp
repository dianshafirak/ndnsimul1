#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/ndnSIM-module.h"
#include <bits/stdc++.h>

namespace ns3 {

std::string topologyFilePath = "src/ndnSIM/examples/topologies/topo-tugas-ndn.txt";
std::string consumerNameInTxt[] = {
		 "Manado", "Palu", "Mamuju", "Kendari",
		 "Ambon", "Tidore", "Buru", "Seram", "Saparua",
		};

int consumerSize = sizeof(consumerNameInTxt)/sizeof(*consumerNameInTxt);

std::string producerNameInTxt[] = {
		 "Makassar"
		};

std::string routerContentStoreSize = "100";
std::string forwardingStrategy = "/localhost/nfd/strategy/best-route";

std::string interestFrequency = "10000";

bool isSimulateIP = false;

int main(int argc, char* argv[]) {

	CommandLine cmd;
	cmd.Parse(argc, argv);

	AnnotatedTopologyReader topologyReader("", 25);
	topologyReader.SetFileName(topologyFilePath);
	topologyReader.Read();


	ndn::StackHelper ndnHelper;
	if (isSimulateIP) {
		routerContentStoreSize = "0";
	};
	//ndnHelper.SetOldContentStore("ns3::ndn::cs::Lru", "MaxSize", routerContentStoreSize);
	ndnHelper.InstallAll();

	ndn::StrategyChoiceHelper::InstallAll("/youtube", forwardingStrategy);

	ndn::GlobalRoutingHelper ndnGlobalRoutingHelper;
	ndnGlobalRoutingHelper.InstallAll();

	Ptr<Node> producerMakassar = Names::Find<Node>("Makassar");

 	ndn::AppHelper producerHelper("ns3::ndn::Producer");
	producerHelper.SetAttribute("PayloadSize", StringValue("1024"));

	ndnGlobalRoutingHelper.AddOrigins("/youtube", producerMakassar);
	producerHelper.SetPrefix("/youtube");
	producerHelper.Install(producerMakassar);

	ndn::AppHelper consumerHelper("ns3::ndn::ConsumerCbr");
  	consumerHelper.SetAttribute("Frequency",
                                 StringValue(interestFrequency));

	Ptr<Node> consumer;

	for (int i=0; i<consumerSize; i++) {
		consumer = Names::Find<Node>(consumerNameInTxt[i]);
		consumerHelper.SetPrefix("/youtube");
		consumerHelper.Install(consumer);
	}

    ndn::L3RateTracer::InstallAll("/tmp/rate-trace.txt", Seconds(1.0));
    L2RateTracer::InstallAll("/tmp/drop-trace.txt", Seconds(1.0));
    ndn::CsTracer::InstallAll("/tmp/cs-trace.txt", Seconds(1.0));
    ndn::AppDelayTracer::InstallAll("/tmp/app-delays-trace.txt");

	ndn::GlobalRoutingHelper::CalculateRoutes();

	Simulator::Stop(Seconds(20.0));

	Simulator::Run();
	Simulator::Destroy();

	return 0;
}

}

int main(int argc, char* argv[]) {
	return ns3::main(argc, argv);
}
