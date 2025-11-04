from django.shortcuts import render
from rest_framework.views import APIView
from rest_framework.response import Response
from rest_framework import status
from .models import SensorData
from .serializers import SensorDataSerializer
from django.http import JsonResponse

# Clasa SensorDataView gestioneaza cererile HTTP GET / POST pentru endpoint-ul REST
class SensorDataView(APIView):
    # returneaza toate datele senzorilor
    def get(self, request):
        sensor_data = SensorData.objects.all().order_by('-timestamp') # cele mai recente valori sunt primele
        serializer = SensorDataSerializer(sensor_data, many=True)
        return Response(serializer.data)

    # primeste datele in format JSON, le valideaza si le salveaza in baza de date
    def post(self, request):
        serializer = SensorDataSerializer(data=request.data)
        if serializer.is_valid():
            serializer.save()
            return Response(serializer.data, status=status.HTTP_201_CREATED)
        return Response(serializer.errors, status=status.HTTP_400_BAD_REQUEST)

# view interfata grafica HTML cu ultimele 100 de inregistrari
def sensor_dashboard(request):
    data = SensorData.objects.all().order_by('-timestamp')[:100] # ultimele 100
    return render(request, 'senzori/sensor_dashboard.html', {'data': data})

# view pentru actualizarea dinamica a datelor in dashboard
def get_new_data(request):
    last_id = request.GET.get("last_id")
    if last_id is None:
        return JsonResponse({"error": "Missing last_id parameter"}, status=400)

    try:
        last_id = int(last_id)
    except ValueError:
        return JsonResponse({"error": "Invalid last_id"}, status=400)

    new_data = SensorData.objects.filter(id__gt=last_id).order_by("timestamp")[:20]
    serialized_data = [
        {
            "id": obj.id,
            "temperature": obj.temperature,
            "humidity": obj.humidity,
            "voc_index": obj.voc_index,
            "timestamp": obj.timestamp.isoformat(),
        }
        for obj in new_data
    ]

    return JsonResponse({"data": serialized_data})
