import { useEffect, useState } from "react";

export default function App() {

  const [data, setData] = useState({
    freeSlots: 0,
    entryCount: 0,
    exitCount: 0,
    parkingStatus: "LOADING",
    updatedTime: ""
  });

  useEffect(() => {

    const fetchData = async () => {

      try {

        const response = await fetch(
          "https://api.thingspeak.com/channels/3375188/feeds/last.json"
        );

        const result = await response.json();

        const freeSlots =
          parseInt(result.field1 || 0);

        const entryCount =
          parseInt(result.field2 || 0);

        const exitCount =
          parseInt(result.field3 || 0);

        const parkingRaw =
          parseInt(result.field4 || 0);

        setData({

          freeSlots,

          entryCount,

          exitCount,

          parkingStatus:
            parkingRaw === 1
              ? "AVAILABLE"
              : "FULL",

          updatedTime:
            result.created_at
        });

      }

      catch (error) {

        console.error(error);
      }
    };

    fetchData();

    const interval =
      setInterval(fetchData, 5000);

    return () =>
      clearInterval(interval);

  }, []);

  return (

    <div className="min-h-screen bg-gray-100 p-6">

      <div className="max-w-6xl mx-auto">

        {/* HEADER */}

        <div className="bg-white rounded-3xl shadow-lg p-6 mb-6">

          <h1 className="text-4xl font-bold text-center">
            ParkSense Dashboard
          </h1>

          <p className="text-center text-gray-500 mt-2">
            Smart Parking IoT Monitoring System
          </p>

        </div>

        {/* MAIN CARDS */}

        <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-4 gap-6">

          {/* FREE SLOTS */}

          <div className="bg-white rounded-3xl shadow-lg p-6 text-center">

            <h2 className="text-xl font-semibold mb-3">
              Free Slots
            </h2>

            <p className="text-5xl font-bold text-green-600">
              {data.freeSlots}
            </p>

          </div>

          {/* ENTRY COUNT */}

          <div className="bg-white rounded-3xl shadow-lg p-6 text-center">

            <h2 className="text-xl font-semibold mb-3">
              Entry Count
            </h2>

            <p className="text-5xl font-bold text-blue-600">
              {data.entryCount}
            </p>

          </div>

          {/* EXIT COUNT */}

          <div className="bg-white rounded-3xl shadow-lg p-6 text-center">

            <h2 className="text-xl font-semibold mb-3">
              Exit Count
            </h2>

            <p className="text-5xl font-bold text-red-600">
              {data.exitCount}
            </p>

          </div>

          {/* PARKING STATUS */}

          <div className="bg-white rounded-3xl shadow-lg p-6 text-center">

            <h2 className="text-xl font-semibold mb-3">
              Parking Status
            </h2>

            <div
              className={`mt-4 text-2xl font-bold px-4 py-3 rounded-2xl ${
                data.parkingStatus === "AVAILABLE"
                  ? "bg-green-100 text-green-700"
                  : "bg-red-100 text-red-700"
              }`}
            >
              {data.parkingStatus}
            </div>

          </div>

        </div>

        {/* LAST UPDATED */}

        <div className="bg-white rounded-3xl shadow-lg p-6 mt-6 text-center">

          <h2 className="text-xl font-semibold mb-2">
            Last Updated
          </h2>

          <p className="text-gray-600">
            {data.updatedTime}
          </p>

        </div>

      </div>

    </div>
  );
}