/*
 * Copyright (c) 2013 ResiliNets, ITTC, University of Kansas
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors: Siddharth Gangadhar <siddharth@ittc.ku.edu>,
 *          Truc Anh N. Nguyen <annguyen@ittc.ku.edu>,
 *          Greeshma Umapathi
 *
 * James P.G. Sterbenz <jpgs@ittc.ku.edu>, director
 * ResiliNets Research Group  https://resilinets.org/
 * Information and Telecommunication Technology Center (ITTC)
 * and Department of Electrical Engineering and Computer Science
 * The University of Kansas Lawrence, KS USA.
 *
 * Work supported in part by NSF FIND (Future Internet Design) Program
 * under grant CNS-0626918 (Postmodern Internet Architecture),
 * NSF grant CNS-1050226 (Multilayer Network Resilience Analysis and Experimentation on GENI),
 * US Department of Defense (DoD), and ITTC at The University of Kansas.
 */

#include "tcp-adaptive-reno.h"

#include "ns3/log.h"
#include "ns3/simulator.h"

NS_LOG_COMPONENT_DEFINE("TcpAdaptiveReno");

namespace ns3
{

NS_OBJECT_ENSURE_REGISTERED(TcpAdaptiveReno);

TypeId
TcpAdaptiveReno::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::TcpAdaptiveReno")
            .SetParent<TcpNewReno>()
            .SetGroupName("Internet")
            .AddConstructor<TcpAdaptiveReno>()
            .AddAttribute(
                "FilterType",
                "Use this to choose no filter or Tustin's approximation filter",
                EnumValue(TcpAdaptiveReno::TUSTIN),
                MakeEnumAccessor(&TcpAdaptiveReno::m_fType),
                MakeEnumChecker(TcpAdaptiveReno::NONE, "None", TcpAdaptiveReno::TUSTIN, "Tustin"))
            .AddTraceSource("EstimatedBW",
                            "The estimated bandwidth",
                            MakeTraceSourceAccessor(&TcpAdaptiveReno::m_currentBW),
                            "ns3::TracedValueCallback::DataRate");
    return tid;
}

TcpAdaptiveReno::TcpAdaptiveReno()
    : TcpWestwoodPlus()
{
    NS_LOG_FUNCTION(this);
}

TcpAdaptiveReno::TcpAdaptiveReno(const TcpAdaptiveReno& sock)
    : TcpWestwoodPlus(sock)
{
    NS_LOG_FUNCTION(this);
    NS_LOG_LOGIC("Invoked the copy constructor");
}

TcpAdaptiveReno::~TcpAdaptiveReno()
{
}

void
TcpAdaptiveReno::PktsAcked(Ptr<TcpSocketState> tcb, uint32_t packetsAcked, const Time& rtt)
{
    NS_LOG_FUNCTION(this << tcb << packetsAcked << rtt);

    if (rtt.IsZero())
    {
        NS_LOG_WARN("RTT measured is zero!");
        return;
    }

    m_ackedSegments += packetsAcked;

    if (rtt.IsZero() || rtt <= m_RttMin)
        m_RttMin = rtt;
    m_Rtt = rtt;
    TcpWestwoodPlus::EstimateBW (rtt, tcb);
}

double
TcpAdaptiveReno::EstimateCongestionLevel()
{
    if (m_RttCongOld < m_RttMin) 
        m_RttCong = m_RttJ;
    else {
        double a = 0.85;
        m_RttCong = Seconds(a * m_RttCongOld.GetSeconds() + (1 - a) * m_RttJ.GetSeconds());
    }
    double c = std::min ( (m_Rtt.GetSeconds() - m_RttMin.GetSeconds()) / (m_RttCong.GetSeconds() - m_RttMin.GetSeconds()), 1.0 );
    return c;
}

void 
TcpAdaptiveReno::EstimateIncWnd(int MSS)
{
    int M = 1000, B = m_currentBW.Get().GetBitRate()/8.0;
    double m_WndIncMax = 1.0 * B / M * MSS, alpha = 10, c = EstimateCongestionLevel();
    double beta = 2 * m_WndIncMax * (1/alpha - (1/alpha + 1)/std::exp(alpha));
    double gamma = 1 - 2 * m_WndIncMax * (1/alpha - (1/alpha + 1/2)/std::exp(alpha));
    m_WndInc = (int)(m_WndIncMax/std::exp(c*alpha) + c*beta + gamma);
}

void
TcpAdaptiveReno::CongestionAvoidance (Ptr<TcpSocketState> tcb, uint32_t segmentsAcked)
{
    if (segmentsAcked == 0) return;
    int MSS = (int)(tcb->m_segmentSize * tcb->m_segmentSize);
    EstimateIncWnd(MSS);
    int Wnd = tcb->m_cWnd.Get();
    m_WndBase += std::max(1.0, 1.0*MSS/Wnd);
    m_WndProbe = std::max (m_WndProbe + 1.0*m_WndInc/Wnd, 0.0);
    Wnd = m_WndBase + m_WndProbe;
    tcb->m_cWnd = Wnd;
}

uint32_t
TcpAdaptiveReno::GetSsThresh(Ptr<const TcpSocketState> tcb, uint32_t bytesInFlight)
{
    m_RttCongOld = m_RttCong, m_RttJ = m_Rtt;
    double c = EstimateCongestionLevel();
    m_WndBase = 1.0 * tcb->m_cWnd / (1+c), m_WndProbe = 0;

    uint32_t ssThresh = std::max ((uint32_t)m_WndBase, 2*tcb->m_segmentSize);

    return ssThresh;
}

Ptr<TcpCongestionOps>
TcpAdaptiveReno::Fork()
{
    return CreateObject<TcpAdaptiveReno>(*this);
}

} // namespace ns3
