#include "data_confirm.hpp"
#include "data_request.hpp"

namespace vanetza
{
namespace geonet
{

DataConfirm& operator ^=(DataConfirm& lhs, DataConfirm::ResultCode rhs)
{
    if (rhs != DataConfirm::ResultCode::Accepted) {
        lhs.result_code = rhs;
    }
    return lhs;
}

DataConfirm::ResultCode validate_data_request(const DataRequest& req, const MIB& mib)
{
    DataConfirm::ResultCode result = DataConfirm::ResultCode::Rejected_Unspecified;

    const auto supported_tcs = mib.vanetzaSupportedTcIds;
    const auto tc_bit = std::uint64_t{1} << req.traffic_class.tc_id().raw();
    if (supported_tcs != 0 && !(supported_tcs & tc_bit)) {
        result = DataConfirm::ResultCode::Rejected_Unsupported_Traffic_Class;
    } else if (req.maximum_lifetime > mib.itsGnMaxPacketLifetime) {
        result = DataConfirm::ResultCode::Rejected_Max_Lifetime;
    } else if (req.repetition && req.repetition->interval < mib.itsGnMinPacketRepetitionInterval) {
        result = DataConfirm::ResultCode::Rejected_Min_Repetition_Interval;
    } else {
        result = DataConfirm::ResultCode::Accepted;
    }

    return result;
}

DataConfirm::ResultCode validate_data_request(const DataRequestWithArea& req, const MIB& mib)
{
    if (area_size(req.destination) > mib.itsGnMaxGeoAreaSize) {
        return DataConfirm::ResultCode::Rejected_Max_Geo_Area_Size;
    } else {
        return validate_data_request(static_cast<const DataRequest&>(req), mib);
    }
}

DataConfirm::ResultCode validate_payload(const std::unique_ptr<DownPacket>& payload, const MIB& mib)
{
    DataConfirm::ResultCode result = DataConfirm::ResultCode::Rejected_Unspecified;

    if (!payload) {
        // leave code to unspecified
    } else if (payload->size() > mib.itsGnMaxSduSize) {
        result = DataConfirm::ResultCode::Rejected_Max_SDU_Size;
    } else {
        result = DataConfirm::ResultCode::Accepted;
    }

    return result;
}

} // namespace geonet
} // namespace vanetza

